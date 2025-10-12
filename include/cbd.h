/**
 * @file kyber_cbd.h
 * @brief Centered Binomial Distribution sampling for Kyber
 * @author Votre Nom
 *
 * Implements SamplePolyCBD from FIPS 203 Algorithm 8
 * Critical for generating noise in lattice-based cryptography
 */

#ifndef KYBER_CBD_H
#define KYBER_CBD_H

#include "kyber_poly.h"

 /*************************************************
  * CBD Sampling (FIPS 203 Algorithm 8)
  *************************************************/

  /**
   * @brief Sample polynomial from centered binomial distribution D_η(R_q)
   *
   * Samples each coefficient from the distribution:
   * f[i] = (sum of η random bits) - (sum of η other random bits)
   *
   * This produces coefficients in the range [-η, η] with binomial distribution
   *
   * @param r Output polynomial with small coefficients
   * @param buf Random bytes (must be 64*η bytes)
   * @param eta Distribution parameter (2 or 3 for ML-KEM-512)
   *
   * Time complexity: O(n), constant-time
   *
   * For ML-KEM-512:
   * - η=3 for secret vectors (128 bytes input)
   * - η=2 for error vectors (128 bytes input)
   *
   * Security note: buf must contain cryptographically secure random bytes
   */
KYBER_API void poly_cbd_eta(poly* r, const uint8_t* buf, int eta);

/**
 * @brief Sample with η=2 (optimized version)
 *
 * @param r Output polynomial
 * @param buf 128 random bytes
 *
 * Time complexity: O(n), constant-time
 */
static inline void poly_cbd2(poly* r, const uint8_t buf[128]) {
    poly_cbd_eta(r, buf, 2);
}

/**
 * @brief Sample with η=3 (optimized version)
 *
 * @param r Output polynomial
 * @param buf 192 random bytes
 *
 * Time complexity: O(n), constant-time
 */
static inline void poly_cbd3(poly* r, const uint8_t buf[192]) {
    poly_cbd_eta(r, buf, 3);
}

/*************************************************
 * Helper functions
 *************************************************/

 /**
  * @brief Load 32-bit little-endian value
  *
  * @param x Pointer to 4 bytes
  * @return 32-bit value
  *
  * Time complexity: O(1), constant-time
  */
static inline uint32_t load32_littleendian(const uint8_t x[4]) {
    uint32_t r;
    r = (uint32_t)x[0];
    r |= (uint32_t)x[1] << 8;
    r |= (uint32_t)x[2] << 16;
    r |= (uint32_t)x[3] << 24;
    return r;
}

/**
 * @brief Load 24-bit little-endian value
 *
 * @param x Pointer to 3 bytes
 * @return 24-bit value (in 32-bit word)
 *
 * Time complexity: O(1), constant-time
 */
static inline uint32_t load24_littleendian(const uint8_t x[3]) {
    uint32_t r;
    r = (uint32_t)x[0];
    r |= (uint32_t)x[1] << 8;
    r |= (uint32_t)x[2] << 16;
    return r;
}

#endif /* KYBER_CBD_H */