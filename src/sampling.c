/**
 * @file sampling.c
 * @brief
 * @author Gabriel Abauzit 
 */

#include "sampling.h"

/**
 * @brief Outputs a pseudorandom NTT
 * @details FIPS 203 Algorithm 7
 * @param r[out] array of length 256
 * @param seed[in] byte array of size 34, corresponds to a 32-byte array and two indices
 */
void sample_ntt(int16_t* r, const uint8_t* seed) {
    int j;
    uint8_t C[3];
    uint16_t d1, d2;
    uint16_t low_bits;
    XOF_ctx *ctx;
    
    j = 0;
    low_bits = (1 << 5) - 1;

    ctx = (XOF_ctx*)malloc(sizeof(XOF_ctx));
    XOF_init(ctx);
    XOF_absorb(ctx, seed);

    while (j < 256) {
        XOF_squeeze(C, ctx, 3);
        d1 = (uint16_t)C[0] + 256 * (C[1] & low_bits);
        d2 = ((uint16_t)C[1] >> 4) + ((uint16_t)C[2] << 4);
        if (d1 < KYBER_Q) {
            r[j++] = d1;
        }
        if (j < 256 && d2 < KYBER_Q) {
            r[j++] = d2;
        }
    }
    
    // Clean sensitive data
    d1 = 0;
    d2 = 0;
    memset(ctx, 0, sizeof(XOF_ctx));
    free(ctx);
}

/**
 * @brief Outputs a pseudorandom sample from the CBD_eta distribution on R_q
 * @details FIPS 203 Algorithm 8
 * @param r[out] array of length 256
 * @param seed[in] byte array of size 64 * eta
 * @param eta[in]
 */
void sample_poly_cbd(int16_t* r, const uint8_t* seed, const uint8_t eta) {
    // It is also possible to read the bits directly from the seed without calling bytes_to_bits().
    // Although this would be more efficient, the algorithm hereunder follows the FIPS 203 specification
    uint8_t b[512 * eta];
    int i, j;
    uint8_t x, y;

    bytes_to_bits(b, seed, 64*eta);
    
    for (i = 0; i < 256; i++) {
        x = 0;
        y = 0;
        for (j = 0; j < eta; j++) {
            x += b[2*i*eta + j];
            y += b[2*i*eta + eta + j];
        }
        r[i] = x - y;
    }
}