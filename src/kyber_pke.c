/**
 * @file kyber_pke.c
 * @brief Implémentation de la partie PKE de Kyber
 * @author Gabriel Abauzit
 * 
 * ATTENTION : Kyber-PKE ne doit PAS être utilisé seul, uniquement comme composant de Kyber-KEM.
 * Il n'y a pas de vérification de validité des entrées ici car les fonctions ci-dessous sont uniquement appelées par Kyber-KEM
 * 
 */


/* Therefore, implementers
shall ensure that intermediate data is destroyed as soon as it is no longer needed. In particular,
for ML-KEM.KeyGen, ML-KEM.Encaps, and ML-KEM.Decaps, only the designated output can be
retained in memory after the algorithm terminates. All other data shall be destroyed prior to
16
FIPS 203 MODULE-LATTICE-BASED KEY-ENCAPSULATION MECHANISM
the algorithm terminating.
There are two exceptions to this rule:
1. The seed (𝑑, 𝑧) generated in steps 1 and 2 of ML-KEM.KeyGen can be stored for later
expansion using ML-KEM.KeyGen_internal. As this seed can be used to compute the
decapsulation key, it is sensitive data and shall be treated with the same safeguards as a
decapsulation key (see SP 800-227 [1]).
2. The matrix 𝐀̂ generated in steps 3-7 of K-PKE.KeyGen (as a subroutine of ML-KEM.KeyGen)
can be stored so that it need not be recomputed in later operations (e.g., ML-KEM.Decaps).
The same matrix 𝐀̂ is also generated in steps 4-8 of K-PKE.Encrypt (as a subroutine of
ML-KEM.Encaps or ML-KEM.Decaps); it can also then be stored. In either case, the matrix
𝐀̂ is data that is easily computed from the public encapsulation key and thus does not
require any special protections. */


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "kyber_pke.h"
#include "poly.h"
#include "polyvec.h"
#include "ntt.h"
#include "cbd.h"
#include "encoding.h"
#include "consts.h"
#include "wrapper.h"

 // Fonctions de hachage externes
extern void G(uint8_t out[64], const uint8_t* in, size_t inlen);
extern void PRF_eta(uint8_t* out, size_t outlen, const uint8_t s[32], uint8_t b);
extern void XOF_init(void);
extern void XOF_absorb(const uint8_t* in, size_t inlen);
extern void XOF_squeeze(uint8_t* out, size_t outlen);


/*
 * Algorithme 13: K-PKE.KeyGen(d)
 * Génération de clés pour K-PKE
 * Input: d ∈ B^32
 * Output: ekPKE ∈ B^(384k+32), dkPKE ∈ B^(384k)
 */

void Kyber_PKE_keygen(uint8_t* ek, uint8_t* dk, const uint8_t d[32]) {
    uint8_t buf[65];  // d || k
    uint8_t rho_sigma[64];
    uint8_t* rho;
    uint8_t* sigma;
    uint8_t seed[34];
    uint8_t N; // pas un int pour éviter des conversions de type
    int i, j;
    poly_t temp;

    polyvec_t A_NTT[KYBER_K];  // Matrice A dans le domaine NTT
    polyvec_t* s_NTT = (polyvec_t*)malloc(sizeof(polyvec_t)); // Allocation dynamique car secret, donc doit être libéré en fin de fonction
    polyvec_t* t_NTT = (polyvec_t*)malloc(sizeof(polyvec_t));
    polyvec_t t_NTT; // N'est pas privé

    memcpy(buf, d, 32);
    buf[32] = KYBER_K;
    G(rho_sigma, buf, 33);

    rho = rho_sigma;      // ρ = rho_sigma[0:32]
    sigma = rho_sigma + 32; // σ = rho_sigma[32:64]

    N = 0;

    // Génération de A dans le domaine NTT
    for (i = 0; i < KYBER_K; i++) {
        for (j = 0; j < KYBER_K; j++) {
            seed[32] = j;
            seed[33] = i;
            sample_NTT(&A_NTT[i].vec[j], seed);
        }
    }

    // Génération du vecteur secret s, stocké dans s_NTT
    for (i = 0; i < KYBER_K; i++) {
        uint8_t prf_output[64 * KYBER_ETA1];
        PRF(&prf_output, 64 * KYBER_ETA1, sigma, N);

    #if KYBER_ETA1 == 2
        sample_poly_cbd_eta2(&(s_NTT->vec[i]), prf_output);
    #else
        sample_poly_cbd_eta3(&(s_NTT->vec[i]), prf_output);
    #endif

        N++;
    }

    // Génération du vecteur d'erreur e, stocké dans e_NTT
    for (i = 0; i < KYBER_K; i++) {
        PRF(&prf_output, 64 * KYBER_ETA1, sigma, N);

    #if KYBER_ETA1 == 2
        sample_poly_cbd_eta2(&(e_NTT->vec[i]), prf_output);
    #else
        sample_poly_cbd_eta3(&(e_NTT->vec[i]), prf_output);
    #endif

        N++;
    }

    polyvec_NTT(s_NTT);
    polyvec_NTT(e_NTT);

    // t_NTT = A_NTT * s_NTT + e_NTT
    polyvec_zero(&t_NTT);
    polyvec_mult(&t_NTT, &A_NTT, s_NTT); // ON MULTIPLIE DES NTT ENTRE ELLES, DONC IL FAUT MULTIPLIER AVEC multiply_NTT

    for (i = 0; i < KYBER_K; i++) {
        for (j = 0; j < KYBER_K; j++) {
            multiplyNTT(&t_NTT, &(A[i].vec[j]), s_NTT + j);//????????????????
        }
    }

    polyvec_add(&t_NTT, &t_NTT, e_NTT);    
    polyvec_reduce(&t_NTT);

    // Encodage de ek
    polyvec_to_bytes(ek, &t_NTT);
    memcpy(ek + KYBER_POLYVECBYTES, rho, 32); //KYBER_POLYVECBYTES = 

    // Encodage de pk
    polyvec_to_bytes(pk, s_NTT);

    polyvec_secure_free(s_NTT);
    polyvec_secure_free(e_NTT);
}


/*
 * Algorithme 14: K-PKE.Encrypt(ekPKE, m, r)
 * Chiffrement K-PKE
 * Input: ekPKE ∈ B^(384k+32), m ∈ B^32, r ∈ B^32
 * Output: c ∈ B^(32(d_u*k+d_v))
 * 
 * takes an encryption key ek, a 32-byte plaintext m, and randomness a as input and produces a single output: a ciphertext c.
 * 
 * 
 */
void Kyber_PKE_encrypt(uint8_t* c, const uint8_t* ek, const uint8_t m[32], const uint8_t r[32]) {
    uint8_t N;
    int i;
    polyvec_t A_TT[KYBER_K];
    polyvec_t t_NTT, y, y_hat, e1, u;
    poly_t e2, v, mu;
    uint8_t rho[32];

    N = 0;

    // Ligne 2: t̂ ← ByteDecode₁₂(ekPKE[0 : 384k])
    polyvec_frombytes(&t_hat, ekPKE);

    // Ligne 3: ρ ← ekPKE[384k : 384k+32]
    memcpy(rho, ekPKE + KYBER_POLYVECBYTES, 32);

    // Lignes 4-8: Régénérer la matrice Â
    gen_matrix_a_hat(a_hat, rho);

    // Lignes 9-12: Générer le vecteur y
    for (i = 0; i < KYBER_K; i++) {
        uint8_t prf_output[64 * KYBER_ETA1];
        PRF_eta(prf_output, 64 * KYBER_ETA1, r, N);

#if KYBER_ETA1 == 2
        sample_poly_cbd_eta2(&y.vec[i], prf_output);
#else
        sample_poly_cbd_eta3(&y.vec[i], prf_output);
#endif

        N++;
    }

    // Lignes 13-16: Générer le vecteur e1
    for (i = 0; i < KYBER_K; i++) {
        uint8_t prf_output[64 * KYBER_ETA2];
        PRF_eta(prf_output, 64 * KYBER_ETA2, r, N);
        poly_cbd_eta2(&e1.vec[i], prf_output);
        N++;
    }

    // Ligne 17: Générer e2
    {
        uint8_t prf_output[64 * KYBER_ETA2];
        PRF_eta(prf_output, 64 * KYBER_ETA2, r, N);
        poly_cbd_eta2(&e2, prf_output);
    }

    // Ligne 18: ŷ ← NTT(y)
    polyvec_ntt(&y_hat, &y);

    // Ligne 19: u ← NTT⁻¹(Â^T ∘ ŷ) + e1
    // On doit calculer Â^T ∘ ŷ
    for (i = 0; i < KYBER_K; i++) {
        polyvec_transpose_basemul_acc_montgomery(&u.vec[i], a_hat, i, &y_hat);
    }
    polyvec_invntt_tomont(&u);
    polyvec_add(&u, &u, &e1);
    polyvec_reduce(&u);

    // Ligne 20: μ ← Decompress₁(ByteDecode₁(m))
    poly_frommsg(&mu, m);

    // Ligne 21: v ← NTT⁻¹(t̂^T ∘ ŷ) + e2 + μ
    polyvec_basemul_acc_montgomery(&v, &t_hat, &y_hat);
    poly_invntt_tomont(&v);
    poly_add(&v, &v, &e2);
    poly_add(&v, &v, &mu);
    poly_reduce(&v);

    // Ligne 22: c1 ← ByteEncode_{d_u}(Compress_{d_u}(u))
    polyvec_compress(c, &u);

    // Ligne 23: c2 ← ByteEncode_{d_v}(Compress_{d_v}(v))
    poly_compress(c + KYBER_POLYVECCOMPRESSEDBYTES, &v);

    // Ligne 24: return c ← (c1‖c2)
    // Déjà fait par l'écriture directe dans c
}











/*************************************/
/* FONCTIONS A VERIFIER, NON ECRITES */
/*************************************/

/*
 * Algorithme 7 : SampleNTT(B)
 * Échantillonne une NTT pseudoaléatoire
 */
static void sample_ntt(poly* a_hat, const uint8_t seed[34])
{
    unsigned int j = 0;
    uint8_t buf[3];
    uint16_t d1, d2;

    XOF_init();
    XOF_absorb(seed, 34);

    while (j < KYBER_N) {
        XOF_squeeze(buf, 3);

        // Lignes 6-7: extraction de deux valeurs de 12 bits
        d1 = buf[0] + 256 * (buf[1] & 0x0F);
        d2 = (buf[1] >> 4) + 16 * buf[2];

        // Lignes 8-11: acceptation si < q
        if (d1 < KYBER_Q) {
            a_hat->coeffs[j++] = d1;
        }

        // Lignes 12-15: même chose pour d2
        if (d2 < KYBER_Q && j < KYBER_N) {
            a_hat->coeffs[j++] = d2;
        }
    }
}



/*
 * Algorithme 15: K-PKE.Decrypt(dkPKE, c)
 * Déchiffrement K-PKE
 * Input: dkPKE ∈ B^(384k), c ∈ B^(32(d_u*k+d_v))
 * Output: m ∈ B^32
 */
void kpke_decrypt(uint8_t m[32], const uint8_t* c, const uint8_t* dkPKE)
{
    polyvec u_prime, s_hat, u_prime_hat;
    poly v_prime, mp, w;

    // Ligne 1: c1 ← c[0 : 32*d_u*k]
    // Ligne 2: c2 ← c[32*d_u*k : 32(d_u*k + d_v)]

    // Ligne 3: u' ← Decompress_{d_u}(ByteDecode_{d_u}(c1))
    polyvec_decompress(&u_prime, c);

    // Ligne 4: v' ← Decompress_{d_v}(ByteDecode_{d_v}(c2))
    poly_decompress(&v_prime, c + KYBER_POLYVECCOMPRESSEDBYTES);

    // Ligne 5: ŝ ← ByteDecode₁₂(dkPKE)
    polyvec_frombytes(&s_hat, dkPKE);

    // Ligne 6: w ← v' - NTT⁻¹(ŝ^T ∘ NTT(u'))
    polyvec_ntt(&u_prime_hat, &u_prime);
    polyvec_basemul_acc_montgomery(&mp, &s_hat, &u_prime_hat);
    poly_invntt_tomont(&mp);

    poly_sub(&w, &v_prime, &mp);
    poly_reduce(&w);

    // Ligne 7: m ← ByteEncode₁(Compress₁(w))
    poly_tomsg(m, &w);
}