/**
 * @file kyber_kem.c
 * @brief Key Exchange Mechanism part of Kyber
 * @author Gabriel Abauzit
 */

#include "kyber_kem.h"

/***********************/
/* INTERNAL ALGORITHMS */
/***********************/

/**
 * @brief Uses randomness to generate an encapsulation key and a corresponding decapsulation key
 * @details FIPS 203 Algorithm 16
 * @param[out] ek byte array of length 384*k + 32
 * @param[out] dk byte array of length 768*k + 96
 * @param[in] d random byte array of length 32
 * @param[in] z random byte array of length 32
 */
void KEM_keygen_internal(uint8_t* ek, uint8_t* dk, const uint8_t* d, const uint8_t* z) {
    uint8_t* hash_ek;

    PKE_keygen(ek, dk, d);
    hash_ek = (uint8_t*)malloc(32);
    H(hash_ek, ek);
    memcpy(dk + 384*KYBER_K, ek, 384*KYBER_K + 32);
    memcpy(dk + 768*KYBER_K + 32, hash_ek, 32);
    memcpy(dk + 768*KYBER_K + 64, z, 32);

    // Clean sensitive data
    memset(hash_ek, 0, 32);
    free(hash_ek);
}

/**
 * @brief Uses the encapsulation key and randomness to generate a key and an associated ciphertext
 * @details FIPS 203 Algorithm 17
 * @param[out] K byte array of length 32
 * @param[out] cipher byte array of length 32 * (d_u*k + d_v)
 * @param[in] ek byte array of length 384*k + 32
 * @param[in] msg random byte array of length 32
 */
void KEM_encaps_internal(uint8_t* K, uint8_t* cipher, const uint8_t* ek, const uint8_t* msg) {
    uint8_t* hash_ek;
    uint8_t* buf; // buf = (msg,hash_ek)
    uint8_t* K_r;
    uint8_t* r;
    
    // Define hash_ek
    hash_ek = (uint8_t*)malloc(32);
    H(hash_ek, ek);

    // Define (msg,hash_ek)
    buf = (uint8_t*)malloc(64);
    memcpy(buf, m, 32);
    memcpy(buf + 32, hash_ek, 32);

    // Define (K,r)
    K_r = (uint8_t*)malloc(64);
    G(K_r, buf);

    r = (int8_t*)malloc(32);
    memcpy(K, K_r, 32);
    memcpy(r, K_r + 32, 32);

    // Encrypt shared secret information
    PKE_encrypt(cipher, ek, msg, r);

    // Clean sensitive data
    memset(hash_ek, 0, 32); free(hash_ek);
    memset(buf, 0, 64); free(buf);
    memset(K_r, 0, 64); free(K_r);
    memset(r, 0, 32); free(r);
}

/**
 * @brief Uses the decapsulation key to produce a shared secret key from a ciphertext.
 * @details FIPS 203 Algorithm 18
 * @param[out] K byte array of length 32
 * @param[in] dk byte array of length 768*k + 96
 * @param[in] cipher byte array of length 32 * (d_u*k + d_v)
 * @return 0 if the decapsulation was successful, 1 otherwise
 */
void KEM_decaps_internal(uint8_t* K, const uint8_t* dk, const uint8_t* cipher) {
    unsigned cipher_len;
    uint8_t* h, z; // hash and randomness from cipher
    uint8_t* msg, msg_h; // msg_h = (msg,h)
    uint8_t* K, r, K_r; // K_r = (K,r) GROSSE ERREUR, IL Y A UNE CONFUSION ENTRE CE K (QUI CORRESPOND AU K' DE FIPS) ET LE K EN ENTREE, IL FAUT ALLER DORMIR
    uint8_t* K_bis;
    uint8_t* z_cipher; // z_cipher = (z,cipher)
    uint8_t* cipher_bis; int i; uint8_t diff; // to test if the decapsulation was successful

    cipher_len = 32 * (KYBER_DU*KYBER_K + KYBER_DV);

    // Recover h
    h = (int8_t*)malloc(32);
    memcpy(h, dk, 768*KYBER_K + 32, 32);

    // Recover z
    z = (int8_t*)malloc(32);
    memcpy(z, dk, 768*KYBER_K + 64, 32);

    // Recover shared secret key
    PKE_decrypt(msg, dk, cipher);

    msg_h = (int8_t*)malloc(64);
    memcpy(msg_h, msg, 32);
    memcpy(msg_h, h, 32);

    K_r = (int8_t*)malloc(64);
    G(K_r, msg_h);

    memcpy(K, K_r, 32);
    r = (int8_t*)malloc(32);
    memcpy(r, K_r + 32, 32);

    // Recover ?????????
    z_cipher = (uint8_t)malloc(32 + cipher_len);
    memcpy(z_cipher, z, 32);
    memcpy(z_cipher, cipher, cipher_len)

    K_bis = (uint8_t*)malloc(32);
    J(K_bis, z_cipher);

    // Test if the decapsulation was successful
    PKE_encrypt(cipher_bis, dk + 384*KYBER_K, msg, r)
    diff = 0;

    for (i = 0; i < cipher_len; i++) {
        diff |= cipher[i] ^ cipher_bis[i];
    }

    if (diff != 0) {
        // Failed decapsulation
        memcpy(K, K_bis, 32);
    }

    // Clean sensitive data
    
}

/***********************/
/* KYBER KEM INTERFACE */
/***********************/