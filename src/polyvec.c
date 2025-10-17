/**
 * @file polyvec.c
 * @brief Opérations vectorielles dans R_q
 * @author Gabriel Abauzit
 *
 **/

#include "polyvec.h"
#include <stdlib.h>

void polyvec_add(polyvec_t *r, const polyvec_t *a, const polyvec_t *b) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
	}
}

void polyvec_sub(polyvec_t *r, const polyvec_t *a, const polyvec_t *b) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_sub(&r->vec[i], &a->vec[i], &b->vec[i]);
	}
}

void polyvec_reduce(polyvec_t *f) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_reduce(&f->vec[i]);
	}
}

void polyvec_zero(polyvec_t *f) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_zero(&f->vec[i]);
	}
}

int polyvec_is_valid(const polyvec_t *f) {
	int i;
	int is_valid = 1;

	for (i = 0; i < KYBER_K; i++) {
		is_valid &= poly_is_valid(&f->vec[i]);
	}

	return is_valid;
}