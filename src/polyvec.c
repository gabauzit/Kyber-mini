/**
 * @file polyvec.c
 * @brief Vectors in R_q
 * @author Gabriel Abauzit
 */

#include "polyvec.h"

/***********************/
/* UTILITARY FUNCTIONS */
/***********************/

/**
 * @brief Initializes a vector to 0
 * @param f
 */
void polyvec_zero(polyvec_t *f) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_zero(&f->vec[i]);
	}
}

/**
 * @brief Checks if all the entries are in their canonical form i.e their coefficients are in [-(q-1)/2,(q-1)/2], in constant time
 * @param f
 * @return 0 if valid, 1 otherwise
 */
int polyvec_is_valid(const polyvec_t *f) {
	int i;
	int is_valid = EXIT_SUCCESS;

	for (i = 0; i < KYBER_K; i++) {
		if (poly_is_valid(&f->vec[i]) == EXIT_FAILURE) {
            is_valid = EXIT_FAILURE;
        }
	}

	return is_valid;
}

/**
 * @brief Reduces all the entries of f modulo q using Barrett reduction
 * @param f
 */
void polyvec_reduce(polyvec_t *f) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_reduce(&f->vec[i]);
	}
}

/**
 * @brief Checks equality between two vectors in constant time
 * @details The polynomial entries should be in their canonical form
 * @param f
 * @param g
 * @return 0 if f = g, 1 otherwise
 */
int polyvec_equal(const polyvec_t* f, const polyvec_t* g) {
    int i;
    int16_t are_equal = EXIT_SUCCESS;

    for (i = 0; i < KYBER_N; i++) {
        if (poly_equal(f->vec[i], g->vec[i]) == EXIT_FAILURE) {
            are_equal = EXIT_FAILURE;
        }
    }

    return are_equal;
}

/**
 * @brief Safely frees up memory space
 * @param ptr points to the pointer to the memory space to free
 */
void polyvec_secure_free(polyvec_t** ptr) {
    if (ptr == NULL || *ptr == NULL) return;

    int i;
    for (i = 0; i < KYBER_K; i++) {
        poly_secure_free(&(*ptr)->vec[i]);
    }

    free(*ptr);
    *ptr = NULL;
}

/**
 * @brief Copies a vector polynomial
 * @param target
 * @param source
 */
void polyvec_copy(polyvec_t *target, const polyvec_t *source) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_copy(&target->vec[i], &source->vec[i]);
	}
}

/*******************/
/* NTT CONVERSIONS */
/*******************/

/**
 * @brief Applies NTT transform to all the entries
 * @param f 
 */
void polyvec_ntt(polyvec_t* f) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        NTT(&f->vec[i]);
    }
}

/**
 * @brief Applies NTT inverse transform to all the entries
 * @param f 
 */
void polyvec_ntt_inv(polyvec_t* f) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        NTT_inv(&f->vec[i]);
    }
}

/*******************************/
/* VECTORIAL OPERATIONS IN R_q */
/*******************************/

/**
 * @brief Vectorial addition in R_q
 *
 * @param r[out]
 * @param a[in]
 * @param b[in]
*/
void polyvec_add(polyvec_t *r, const polyvec_t *a, const polyvec_t *b) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
	}
}

/**
 * @brief Vectorial subtraction in R_q
 *
 * @param r[out]
 * @param a[in]
 * @param b[in]
 *
 */
void polyvec_sub(polyvec_t *r, const polyvec_t *a, const polyvec_t *b) {
	int i;

	for (i = 0; i < KYBER_K; i++) {
		poly_sub(&r->vec[i], &a->vec[i], &b->vec[i]);
	}
}

/**
 * @brief Computes the scalar product of two vectors
 * 
 * @param r[out]
 * @param a[in]
 * @param b[in]
 */
void polyvec_scalar_product(poly_t* r, const polyvec_t* a, const polyvec_t* b) {
    int i;
    poly_t temp;

    poly_zero(r);
    
    for (i = 0; i < KYBER_K; i++) {
        poly_mult(&temp, &a->vec[i], &b->vec[i]);
        poly_add(r, r, &temp);
    }

    poly_zero(&temp); // At the end of the loop, temp contains a->vec[KYBER_K-1] * b->vec[KYBER_K-1], if a and b are private we shall erase this value
}

/**
 * @brief Computes a matrix/vector product
 * 
 * @param r[out]
 * @param A[in] matrix of size KYBER_K * KYBER_K
 * @param v[in] vector applied to A, of size KYBER_K
 */
void polyvec_product(polyvec_t* r, const polyvec_t** A, const polyvec_t* v) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        polyvec_scalar_product(&r->vec[i], A[i], v);
    }
}