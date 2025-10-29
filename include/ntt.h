/**
 * @file ntt.h
 * @brief Implementation of the NTT related algorithms
 * @author Gabriel Abauzit
 */

#ifndef NTT_H
#define NTT_H

#include <stdint.h>
#include "reduce.h"

/*****************************************************************************************************/
/* All NTT calculations (which therefore involve multiplication) take place in the Montgomery domain */
/* for optimization reasons. In particular, the zeta tables in ntt.c are in the Montgomery domain.   */
/*****************************************************************************************************/

void NTT(int16_t f[256]);

void NTT_inv(int16_t f[256]);

void BaseCaseMultiply(int16_t* r0, int16_t* r1, const int16_t* a0, const int16_t* a1, const int16_t* b0, const int16_t* b1, const int16_t* m);

void NTT_multiply(int16_t r[256], const int16_t a[256], const int16_t b[256]);

#endif