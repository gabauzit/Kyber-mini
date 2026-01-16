/**
 * @file poly.h
 * @brief Implementation of polynomials in R_q
 * @author Gabriel Abauzit
 */

#include "poly.h"

/***********************/
/* UTILITARY FUNCTIONS */
/***********************/

/**
 * @brief Sets all coefficients to 0
 * @param f
 */
void poly_zero(poly_t* f) {
    int i;
    volatile int16_t* ptr = f->coeffs; // volatile to prevent an optimisation of compiler, important when freeing a memory space with poly_secure_free;

    for (i = 0; i < KYBER_N; i++) {
        ptr[i] = 0;
    }
}

/**
 * @brief Checks if the coefficients are in their canonical form, that is in [-(q-1)/2,(q-1)/2], in constant time
 * @param f
 * @return 0 if valid, 1 otherwise
 */
int poly_is_valid(const poly_t* f) {
    int i;
    int is_valid = 1;
    int16_t temp;

    for (i = 0; i < KYBER_N; i++) {
        temp = f->coeffs[i] + (KYBER_Q >> 1);
        is_valid &= (temp >= 0) & (temp < KYBER_Q);
    }

    return 1 - is_valid;
}

/**
 * @brief Reduces all the coefficients into their canonical form i.e in [-(q-1)/2,(q-1)/2]
 */
void poly_reduce(poly_t* f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = barrett_reduce(f->coeffs[i]);
    }
}

/**
 * @brief Checks equality between two polynomials in constant time
 * @details The polynomial entries should be in their canonical form
 * @return 0 if f = g, 1 otherwise
 */
int poly_equal(const poly_t* f, const poly_t* g) {
    int i;
    int16_t are_equal = 0;
    int16_t temp;

    for (i = 0; i < KYBER_N; i++) {
        // To test equality, we first need to ensure that all the mod-q coefficients are in [-(q-1)/2,(q-1)/2]
        // barrett_reduce ensures this when its entry lies within a reasonable range around 0.
        temp = barrett_reduce(f->coeffs[i] - g->coeffs[i]);
        are_equal |= temp;
    }

    return (are_equal != 0);
}

/**
 * @brief Safely frees up memory space
 * @param ptr points to the pointer to the memory space to free
 */
void poly_secure_free(poly_t** ptr) {
    if (ptr == NULL || *ptr == NULL) return;

    poly_zero(*ptr); 
    free(*ptr);
    *ptr = NULL;
} 

/**
 * @brief Copies a polynomial
 */
void poly_copy(poly_t* target, const poly_t* source) {
    memcpy(target->coeffs, source->coeffs, KYBER_N * sizeof(int16_t));
}

/********************************/
/* MONTGOMERY REDUCTIONS IN R_q */
/********************************/

/**
 * @brief Sends all the coefficients into Montgomery domain
 */
void poly_to_montgomery(poly_t* f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = fqmul(f->coeffs[i], 1353); // a * R = (a * R^2) * R^{-1}, since R = 2^16, we have R^2 = 1353 (mod q)
    }
}

/**
 * @brief Applies Montgomery reduction to all the coefficients
 */
void poly_from_montgomery(poly_t* f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = montgomery_reduce(f->coeffs[i]);
    }
}

/********************************/
/* ARITHMETIC OPERATIONS IN R_q */
/********************************/

/**
 * @brief Addition in R_q
 */
void poly_add(poly_t* r, const poly_t* a, const poly_t* b) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = barrett_reduce(a->coeffs[i] + b->coeffs[i]);
    }
}

/**
 * @brief Subtraction in R_q
 */
void poly_sub(poly_t* r, const poly_t* a, const poly_t* b) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        r->coeffs[i] = barrett_reduce(a->coeffs[i] - b->coeffs[i]);
    }
}

/**
 * @brief Fast multiplication in R_q using NTT
 */
void poly_mult(poly_t* r, const poly_t* a, const poly_t* b) {
    poly_t* a_copy = (poly_t*)malloc(sizeof(poly_t));
    poly_t* b_copy = (poly_t*)malloc(sizeof(poly_t));

    poly_copy(a_copy, a);
    poly_copy(b_copy, b);

    poly_to_montgomery(a_copy);
    poly_to_montgomery(b_copy);

    NTT(a_copy->coeffs);
    NTT(b_copy->coeffs);
    NTT_multiply(r->coeffs, a_copy->coeffs, b_copy->coeffs);

    poly_secure_free(&a_copy);
    poly_secure_free(&b_copy);

    NTT_inv(r->coeffs);
    poly_from_montgomery(r);
    poly_reduce(r);
}

/*********************************/
/* COMPRESSION AND DECOMPRESSION */
/*********************************/

/**
 * @brief Compresses all the coefficients of f
 * @param f 
 */
void poly_compress(poly_t* f, const unsigned d) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = compress(f->coeffs[i], d);
    }
}

/**
 * @brief Decompresses all the coefficients of f
 * @param f 
 */
void poly_decompress(poly_t* f, const unsigned d) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = decompress(f->coeffs[i], d);
    }
}