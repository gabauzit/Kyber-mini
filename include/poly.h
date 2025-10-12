/*
 * @file poly.h
 * @brief Implémentation du type poly_t et de ses opérations usuelles
 * @author Gabriel Abauzit
 *
 */

#ifndef KYBER_POLY_H
#define KYBER_POLY_H

#include <stdint.h>
#include <string.h> // pour memcpy
#include "consts.h"

/**
 * @brief Un polynôme de R_q est représenté par le tableau de ses coefficients :
 * Le polynôme f = f_0 + f_1*x + ... + f_255*x^255 est représenté par le tableau f[i] = f_i
 */

typedef struct {
    int16_t coeffs[KYBER_N];
} poly_t;

/*********************************************/
/* Opérations de réduction en temps constant */
/*********************************************/

/**
 * @brief Réduction de Montgomery
 *
 * @param int16_t a dans [-q*2^15, q*2^15]
 * @return la réduction de Montgomery de a : a*R^-1 mod q où R=2^16
 *
 */

static inline int16_t montgomery_reduce(int32_t a) {
    int16_t t = (int16_t)(a * KYBER_MONT_INV);  // KYBER_MONT_INV = -q^{-1} mod 2^16
    t = (int16_t)((a - (int32_t)t * KYBER_Q) >> 16);
    return t;
}

/**
 * @brief Réduction de Barrett
 *
 * @param int16_t a
 * @return le reste de a modulo q dans [0,q-1]
 *
 */

static inline int16_t barrett_reduce(int16_t a) {
    int16_t t;
    const int16_t v = 20158; // floor(2^26 / q) = 20158 A VOIR SI ON REMPLACE PAR L'ENTIER LE PLUS PROCHE AUQUEL CAS v = 20159

    t = (int16_t)(((int32_t)v * a + (1 << 25)) >> 26);
    t *= KYBER_Q;
    return a - t;
}

/**
 * @brief Réduction conditionnelle modulo q
 *
 * @param int16_t a dans [0,2q-1]
 * @return a mod q dans [0,q-1]
 *
 */

static inline int16_t cond_reduce(int16_t a) {
    a -= KYBER_Q;
    a += (a >> 15) & KYBER_Q;
    return a;
}

/**
 * @brief Multiplication dans F_q via la réduction de Montgomery, si a et b sont dans le domaine de Montgomery, calcule leur produit dans le domaine de Montgomery
 *        Autrement dit, si en entrée on a a*R et b*R, renvoie a*b*R
 *
 * @param int16_t a : réduction de Montgomery de a A VOIR, COMMENT SONT CHOISIS LES ENTREES, DANS LE DOMAINE DE MONTGOMERY OU NON
 * @param int16_t b : réduction de Montgomery de b
 * @return la réduction de Montgomery de a*b
 *
 */
static inline int16_t fqmul(int16_t a, int16_t b) {
    return montgomery_reduce((int32_t)a * b);
}

/***********************/
/* OPERATIONS DANS R_q */
/***********************/

/**
 * @brief Addition dans R_q
 *
 * @param r : résultat
 * @param a : première opérande
 * @param b : seconde opérande
 * 
*/

void poly_add(poly_t *r, const poly_t *a, const poly_t *b);

/**
 * @brief Soustraction dans R_q
 *
 * @param r : résultat
 * @param a : première opérande
 * @param b : seconde opérande
 *
 */

void poly_sub(poly_t *r, const poly_t *a, const poly_t *b);

/**
 * @brief Réduit les coefficients de f modulo q via la réduction de Barrett
 *
 * @param f
 *
 */

void poly_reduce(poly_t *f);

/**
 * @brief Conversion dans le domaine de Montgomery : r = a * R mod q
 *
 * @param r Polynomial in Montgomery form
 * @param a Input polynomial
 *
 * Time complexity: O(n), constant-time
 */

/*************************/
/* FONCTIONS UTILITAIRES */
/*************************/

/**
 * @brief Intialise un élément de R_q à 0
 *
 * @param f
 *
 */

void poly_zero(poly_t *f);

/**
 * @brief Vérifie si les coefficients sont dans [0,q-1]
 *
 * @param f
 * @return 1 si valide, 0 sinon
 *
 */

int poly_is_valid(const poly_t *f);

/**
 * @brief Vérifie l'égalité en temps constant, les entrées sont présupposées à coefficients dans [0,q-1]
 *
 * @param f
 * @param g
 * @return 1 si f = g, 0 sinon
 *
 */

int poly_equal(const poly_t* f, const poly_t* g);

/**
 * @brief Copie dans R_q
 *
 * @param target 
 * @param source
 *
 */

static inline void poly_copy(poly_t *target, const poly_t *source) {
    memcpy(target->coeffs, source->coeffs, KYBER_N * sizeof(int16_t));
}

#endif