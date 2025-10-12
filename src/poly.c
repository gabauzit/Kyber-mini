/**
 * @file poly.c
 * @brief Implémentation du type poly_t et de ses opérations usuelles
 * @author Gabriel Abauzit
 * 
 **/

#include "poly.h"

/***********************/
/* OPERATIONS DANS R_q */
/***********************/

// ATTENTION : pour des raisons d'optimisation, on ne réduit pas les coefficients après chaque calcul, ceci nous expose à un débordement du type 16int_t.
//             Si les coefficients sont présupposés réduits, on peut effectuer 8 additions / soustractions de façon sûre, au-delà les calculs peuvent être faussés

void poly_add(poly_t *r, const poly_t *a, const poly_t *b) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
    }
}

void poly_sub(poly_t *r, const poly_t *a, const poly_t *b) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
    }
}

void poly_reduce(poly_t *f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = barrett_reduce(f->coeffs[i]);
    }
}

/*************************/
/* Fonctions utilitaires */
/*************************/

void poly_zero(poly_t *f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = 0;
    }
}

int poly_is_valid(const poly_t *f) {
    int i;  
    int is_valid = 1;

    for (i = 0; i < KYBER_N; i++) {
        int16_t c = f->coeffs[i];
        is_valid &= (c >= 0) & (c < KYBER_Q);
    }

    return is_valid;
}

int poly_equal(const poly_t* f, const poly_t* g) {
    int i;
    int are_equal = 1;

    for (i = 0; i < KYBER_N; i++) {
        are_equal &= (f->coeffs[i] == g->coeffs[i]);
    }

    return are_equal;
}