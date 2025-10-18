/**
 * @file polyvec.h
 * @brief Vectors in R_q
 * @author Gabriel Abauzit
 */

#ifndef POLYVEC_H
#define POLYVEC_H

#include "poly.h"

typedef struct {
	poly_t vec[KYBER_K];
} polyvec_t;

/***********************/
/* UTILITARY FUNCTIONS */
/***********************/

void polyvec_zero(polyvec_t *f);

int polyvec_is_valid(const polyvec_t* f);

void polyvec_reduce(polyvec_t* f);

void polyvec_equal(const polyvec_t* f, const polyvec_t* g);

void polyvec_secure_free(polyvec_t** ptr);

void polyvec_copy(polyvec_t *target, const polyvec_t *source);

/*******************/
/* NTT CONVERSIONS */
/*******************/

void polyvec_ntt(polyvec_t* f);

void polyvec_ntt_inv(polyvec_t* f);

/*******************************/
/* VECTORIAL OPERATIONS IN R_q */
/*******************************/

void polyvec_add(polyvec_t *r, const polyvec_t *a, const polyvec_t *b);

void polyvec_sub(polyvec_t *r, const polyvec_t *a, const polyvec_t *b);

void polyvec_scalar_product(poly_t* r, const polyvec_t* a, const polyvec_t* b);

void polyvec_product(polyvec_t* r, const polyvec_t** A, const polyvec_t* v);

#endif