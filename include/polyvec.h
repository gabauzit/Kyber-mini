/**
 * @file polyvec.h
 * @brief Opérations vectorielles dans R_q
 * @author Gabriel Abauzit
 * 
 **/

#ifndef KYBER_POLYVEC_H
#define KYBER_POLYVEC_H

#include "poly.h"
#include "consts.h"

typedef struct {
	poly_t vec[KYBER_K];
} polyvec_t;

/**
 * @brief Addition vectorielle dans R_q
 *
 * @param r : résultat
 * @param a : première opérande
 * @param b : seconde opérande
 *
*/

void polyvec_add(polyvec_t *r, const polyvec_t *a, const polyvec_t *b);

/**
 * @brief Soustraction vectorielle dans R_q
 *
 * @param r : pointeur vers le résultat
 * @param a : première opérande
 * @param b : seconde opérande
 *
 */

void polyvec_sub(polyvec_t *r, const polyvec_t *a, const polyvec_t *b);

/**
 * @brief Réduit les entrées de f modulo q via la réduction de Barrett
 *
 * @param f
 *
 */

void polyvec_reduce(polyvec_t *f);

/**
 * @brief Intialise un vecteur de R_q à 0
 *
 * @param f
 *
 */

void polyvec_zero(polyvec_t *f);

/**
 * @brief Vérifie si les entrées sont valides sont dans [0,q-1]
 *
 * @param f
 * @return 1 si valide, 0 sinon
 *
 */

int polyvec_is_valid(const polyvec_t* f);

void polyvec_secure_free(polyvec_t* f);
}

/**
 * @brief Copie un vecteur de R_q
 *
 * @param target
 * @param source
 *
 */

static inline void polyvec_copy(polyvec_t *target, const polyvec_t *source) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_copy(&target->vec[i], &source->vec[i]);
	}
}

/* A FAIREEEEEEEEEEEEEEEEEEEEE */

// Algorithme 13

void polyvec_NTT(polyvec_t* f);

void polyvec_mult(polyvec_t* r, polyvec_t* A[KYBER_K], polyvec_t* v); // Calcule le produit matriciel A*v

void polyvec_to_bytes(uint8_t* r, polyvec_t* f);

#endif