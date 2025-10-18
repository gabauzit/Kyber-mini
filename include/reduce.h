#ifndef REDUCE_H
#define REDUCE_H

/**
 * @file reduce.h
 * @brief Reduction functions in constant time
 * @author : Gabriel Abauzit
 */

#include <stdint.h>
#include "consts.h"

/**
 * @brief Barrett reduction
 *
 * @param int16_t a Input integer to be reduced 
 * @return A 16 bit integer that is the mod-q reduction of a in [-(q-1)/2,(q-1)/2]
 */
/*static inline int16_t barrett_reduce(int16_t a) {
  int16_t t;
  const int16_t v = BARRETT_FACTOR;

  t  = ((int32_t)v*a + (1<<25)) >> 26;
  t *= KYBER_Q;
  return a - t;
} */

static inline int16_t barrett_reduce(int16_t a) {
    int32_t r = a % KYBER_Q; // reste modulo q

    // recaler dans l'intervalle centré
    if (r > (KYBER_Q - 1) / 2) {
        r -= KYBER_Q;
    } else if (r < -(KYBER_Q - 1) / 2) {
        r += KYBER_Q;
    }

    return (int16_t)r;
}

/**
 * @brief Montgomery reduction
 * @param int32_t a : Input integer to be reduced, has to be in {-q*2^15,...,q*2^15-1}
 * @return A 16-bit integer congruent to a * R^{-1} (mod q), where R=2^16
 */
static inline int16_t montgomery_reduce(int32_t a) {
    int16_t t;

    t = (int16_t)(a * MONTGOMERY_QINV);  // MONTGOMERY_QINV = 62209 = q^{-1} mod 2^16
    t = (a - (int32_t)t * KYBER_Q) >> 16;
    return barrett_reduce(t);
}

/**
 * @brief Multiplication in F_q in the Montgomery domain
 * @details Suppose a represents x in the Montgomery domain, and b represents y, this means that a = x * R (mod q) and b = y * R (mod q).
 *          In the Montgomery domain, x * y is represented by (x * y) * R = (a * b) * R^{-1}, this is the Montgomery reduction of a * b
 * @param int16_t a
 * @param int16_t b
 * @return Montgomery reduction of a * b
 */
static inline int16_t fqmul(int16_t a, int16_t b) {
    return montgomery_reduce((int32_t)a * b);
}

#endif