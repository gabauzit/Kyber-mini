/**
 * @file poly.h
 * @brief Implementation of polynomials in R_q
 * @author Gabriel Abauzit
 */

#ifndef KYBER_POLY_H
#define KYBER_POLY_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "consts.h"
#include "reduce.h"
#include "ntt.h"

// poly_t f represents the polynomial f_0 + f_1*x + ... + f_255*x^255 where f_i = f.coeffs[i].
// The canonical representants of the coefficients modulo q are in [-(q-1)/2,(q-1)/2].
typedef struct {
    int16_t coeffs[KYBER_N];
} poly_t;

/***********************/
/* UTILITARY FUNCTIONS */
/***********************/

void poly_zero(poly_t* f);

int poly_is_valid(const poly_t* f);

void poly_reduce(poly_t* f);

int poly_equal(const poly_t* f, const poly_t* g);

void poly_secure_free(poly_t** f);

void poly_copy(poly_t* target, const poly_t* source);

/********************************/
/* MONTGOMERY REDUCTIONS IN R_q */
/********************************/

void poly_to_montgomery(poly_t* f);

void poly_from_montgomery(poly_t* f);

/********************************/
/* ARITHMETIC OPERATIONS IN R_q */
/********************************/

void poly_add(poly_t* r, const poly_t* a, const poly_t* b);

void poly_sub(poly_t* r, const poly_t* a, const poly_t* b);

void poly_mult(poly_t* r, const poly_t* a, const poly_t* b);

#endif