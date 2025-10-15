/**
 * @file polyvec.c
 * @brief Opérations vectorielles dans R_q
 * @author Gabriel Abauzit
 *
 **/

#include "polyvec.h"

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

void polyvec_secure_free(polyvec_t* f) {

}


void polyvec_secure_free(polyvec_t* f) {
	if (f == NULL) return;

	int i;

	for (i = 0; i < KYBER_N; i++) {
		poly_secure_free(&(f.vec[i]));
	}

	free(f);
}

/* A FAIREEEEEEEEEEEEEEEEEEEEE */

// Algorithme 13

void polyvec_NTT(polyvec_t* f) {

}

void polyvec_mult(polyvec_t* r, polyvec_t* A[KYBER_K], polyvec_t* v) { // Calcule le produit matriciel A*v

}