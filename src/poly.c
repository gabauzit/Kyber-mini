/**
 * @file poly.c
 * @brief Impl�mentation du type poly_t et de ses op�rations usuelles
 * @author Gabriel Abauzit
 *
 **/

#include "poly.h"

 /***********************/
 /* OPERATIONS DANS R_q */
 /***********************/

 // ATTENTION : pour des raisons d'optimisation, on ne r�duit pas les coefficients apr�s chaque calcul, ceci nous expose � un d�bordement du type 16int_t.
 //             Si les coefficients sont pr�suppos�s r�duits, on peut effectuer 8 additions / soustractions de fa�on s�re, au-del� les calculs peuvent �tre fauss�s

void poly_add(poly_t* r, const poly_t* a, const poly_t* b) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
    }
}

void poly_sub(poly_t* r, const poly_t* a, const poly_t* b) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
    }
}

void poly_mult(poly_t* r, poly_t* a, poly_t* b) {
    poly_to_montgomery(a);
    poly_to_montgomery(b);

    NTT(a);
    NTT(b);
    MultiplyNTT(r, a, b);
    NTT_inv(r);

    poly_from_montgomery(r);
    poly_from_montgomery(a);
    poly_from_montgomery(b);

    poly_reduce(r);
    poly_reduce(a);
    poly_reduce(b);
}

void poly_reduce(poly_t* f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = barrett_reduce(f->coeffs[i]);
    }
}

/*************************/
/* Fonctions utilitaires */
/*************************/

void poly_zero(poly_t* f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = 0;
    }
}

int poly_is_valid(const poly_t* f) {
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

void poly_secure_free(poly_t *f) {
    if (f == NULL) return;

    volatile int16_t* ptr = f->coeffs;
    for (i = 0; i < KYBER_N; i++)
        ptr[i] = 0;

    free(f);
}