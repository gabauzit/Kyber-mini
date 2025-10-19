/**
 * @file kyber_pke.c
 * @brief Public Key Encryption part of Kyber (should NOT be used independently on its own)
 * @author Gabriel Abauzit
 */

#include "kyber_pke.h"

/**
 * @brief Generates encryption and decryption keys
 * @details FIPS 203 Algorithm 13
 * @param[out] ek byte array of length 384*k + 32
 * @param[out] dk byte array of length 384*k
 * @param[in] d random byte array of length 32
 */
void PKE_keygen(uint8_t* ek, uint8_t* dk, const uint8_t* d) {
    uint8_t i, j, N;
    polyvec_t A[KYBER_K];
    polyvec_t *s, *e, *t;
    uint8_t buf[64 * KYBER_ETA1];
    uint8_t rho[32], sigma[32], rho_sigma[64]; // seeds to generate A, s, e
    uint8_t c[33]; // seed to generate (rho, sigma)
    uint8_t seed_A[34]; // temporary seed to generate the entries of A

    memcpy(c, d, 32);
    c[32] = k;
    G(rho_sigma, c);
    memcpy(rho, rho_sigma, 32);
    memcpy(sigma, rho_sigma + 32, 32);

    N = 0;

    // Generate A
    memcpy(seed_A, rho, 32);
    for (i = 0; i < KYBER_K; i++) {
        for (j = 0; j < KYBER_K; j++) {
            seed_A[32] = i;
            seed_A[33] = j;
            sample_ntt(A[i].vec[j].coeffs, seed_A);
        }
    }

    // Generate s
    s = (polyvec_t*)malloc(sizeof(polyvec_t));
    for (i = 0; i < KYBER_K; i++) {
        PRF(buf, KYBER_ETA1, sigma, N);
        sample_poly_cbd(s->vec[i].coeffs, buf, KYBER_ETA1);
        N++;
    }

    // Generate e
    e = (polyvec_t*)malloc(sizeof(polyvec_t));
    for (i = 0; i < KYBER_K; i++) {
        PRF(buf, KYBER_ETA1, sigma, N);
        sample_poly_cbd(e->vec[i].coeffs, buf, KYBER_ETA1);
        N++;
    }

    // Compute t
    t = (polyvec_t*)malloc(sizeof(polyvec_t));
    polyvec_ntt(s);
    polyvec_ntt(e);
    polyvec_ntt_product(t, A, s);
    polyvec_add(t, e);

    // Construct ek and dk
    for (i = 0; i < KYBER_K; i++) {
        // 384 = 32 * 12
        byte_encode(ek + 384*i, t->vec[i].coeffs, 12);
        byte_encode(dk + 384*i, s->vec[i].coeffs, 12);
    }
    memcpy(ek + 384 * KYBER_K, rho, 32);

    // Clean sensitive data
    polyvec_secure_free(*s);
    polyvec_secure_free(*e);
    polyvec_secure_free(*t);
    memset(sigma, 0, 32);
    memset(rho_sigma, 0, 64);
    memset(buf, 0, 64 * KYBER_ETA1);
    memset(c, 0, 33);
}

/**
 * @brief Uses the encryption key to encrypt a plaintext message using the randomness of r
 * @details FIPS 203 Algorithm 14
 * @param[out] cipher byte array of length 32 * (d_u*k + d_v)
 * @param[in] ek byte array of length 384*k + 32
 * @param[in] msg byte array of length 32
 * @param[in] random random byte array of length 32
 */
void PKE_encrypt(uint8_t* cipher, const uint8_t* ek, const uint8_t* msg, const uint8_t* random) {
    uint8_t i, j, N;
    polyvec_t *t, *y, *e1;
    poly_t *e2, *mu;
    polyvec_t u;
    poly_t v;
    uint8_t buf_eta1[64 * KYBER_ETA1], buf_eta2[64 * KYBER_ETA2];
    uint8_t rho[32]; // seed to generate A
    uint8_t seed_A[34]; // temporary seed to generate the entries of A
    polyvec_t A[KYBER_K];
    uint8_t c1[32 * KYBER_DU * KYBER_K], c2[32 * KYBER_DV]; // ciphertext

    N = 0;

    // Recover t
    t = (polyvec_t*)malloc(sizeof(polyvec_t));
    polyvec_byte_decode(t, ek, 12);

    // Recover rho
    memcpy(rho, ek + 384*KYBER_K, 32);

    // Recover A from its seed rho
    memcpy(seed_A, rho, 32);
    for (i = 0; i < KYBER_K; i++) {
        for (j = 0; j < KYBER_K; j++) {
            seed_A[32] = i;
            seed_A[33] = j;
            sample_ntt(A[i].vec[j].coeffs, seed_A);
        }
    }

    // Generate a random polyvec_t y
    y = (polyvec_t*)malloc(sizeof(polyvec_t));
    for (i = 0; i < KYBER_K; i++) {
        PRF(buf_eta1, KYBER_ETA1, random, N);
        sample_poly_cbd(y->vec[i].coeffs, buf_eta1, KYBER_ETA1);
        N++;
    }

    // Generate the random noise polyvec_t e1
    e1 = (polyvec_t*)malloc(sizeof(polyvec_t));
    for (i = 0; i < KYBER_K; i++) {
        PRF(buf_eta2, KYBER_ETA2, random, N);
        sample_poly_cbd(e1->vec[i].coeffs, buf_eta2, KYBER_ETA2);
        N++;
    }

    // Generate the random noise poly_t e2
    e2 = (poly_t*)malloc(sizeof(poly_t));
    PRF(buf_eta2, KYBER_ETA2, random, N);
    sample_poly_cbd(e2->coeffs, buf_eta2, KYBER_ETA2);

    // Compute u
    polyvec_ntt(y);
    polyvec_transpose(&A);
    polyvec_ntt_product(&u, &A, y);
    polyvec_ntt_inv(&u);
    polyvec_add(&u, &u, e1);

    // Compute v
    mu = (poly_t*)malloc(sizeof(poly_t));
    byte_decode(mu->coeffs, msg, 1);
    poly_decompress(mu, 1);
    polyvec_ntt_scalar_product(&v, t, y);
    NTT_inv(v.coeffs);
    poly_add(&v, &v, e2);
    poly_add(&v, &v, mu);

    // Encode ciphertext
    polyvec_compress(&u, KYBER_DU);
    polyvec_byte_encode(c1, &u, KYBER_DU);
    poly_compress(&v, KYBER_DV);
    polyvec_byte_encode(&v, KYBER_DV);

    memcpy(cipher, c1, 32 * KYBER_DU * KYBER_K);
    memcpy(cipher + 32 * KYBER_DU * KYBER_K, c2, 32 * KYBER_DV);

    // Clean sensitive data
    polyvec_secure_free(*t);
    polyvec_secure_free(*y);
    polyvec_secure_free(*e1)
    poly_secure_free(*e2);
    poly_secure_free(*mu);
    memset(buf_eta1, 0, 64 * KYBER_ETA1);
    memset(buf_eta2, 0, 64 * KYBER_ETA2);
}

/**
 * @brief Uses the decryption key to decrypt a ciphertext
 * @details FIPS 203 Algorithm 15
 * @param[out] msg byte array of length 32
 * @param[in] dk byte array of length 384 * k
 * @param[in] cipher byte array of length 32 * (d_u*k + d_v)
 */
void PKE_decrypt(uint8_t* msg, const uint8_t* dk, const uint8_t* cipher) {
    uint8_t c1[32 * KYBER_DU * KYBER_K], c2[32 * KYBER_DV]; // ciphertext
    polyvec_t u;
    poly_t v;
    poly_t *w;
    polyvec_t* s;

    memcpy(c1, cipher, 32 * KYBER_DU * KYBER_K);
    memcpy(c2, cipher + 32 * KYBER_DU * KYBER_K, 32 * KYBER_DV);

    // Recover an approximation of u
    polyvec_byte_decode(u, c1, KYBER_DU);
    polyvec_decompress(u, KYBER_DU);

    // Recover an approximation of v
    byte_decode(v.coeffs, c2, KYBER_DV);
    poly_decompress(v, KYBER_DV);

    // Recover the secret s
    s = (polyvec_t*)malloc(sizeof(polyvec_t));

    // Compute w
    w = (poly_t*)malloc(sizeof(poly_t));
    polyvec_ntt(&u);
    polyvec_ntt_scalar_product(w, s, &u);
    poly_sub(w, &v, w);

    // Recover msg
    poly_compress(w, 1);
    byte_encode(msg, w->coeffs, 1);

    // Clean sensitive data
    polyvec_secure_free(*s);
    poly_secure_free(*w);
}