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

// The following two functions take place inside the NTT domain.
// Scalar products and matrix-vector products always take place inside the NTT domain in Kyber

/**
 * @brief Computes the scalar product of two vectors inside NTT domain
 * 
 * @param r[out]
 * @param a[in]
 * @param b[in]
 */
void polyvec_ntt_scalar_product(poly_t* r, const polyvec_t* a, const polyvec_t* b) {
    int i;
    poly_t temp;

    poly_zero(r);
    
    for (i = 0; i < KYBER_K; i++) {
        NTT_multiply(temp.coeffs, a->vec[i].coeffs, b->vec[i].coeffs);
        poly_add(r, r, &temp);
    }

    poly_zero(&temp); // At the end of the loop, temp contains a->vec[KYBER_K-1] * b->vec[KYBER_K-1], if a and b are private we shall erase this value
}

/**
 * @brief Computes a matrix/vector product inside NTT domain
 * 
 * @param r[out]
 * @param A[in] matrix of size KYBER_K * KYBER_K
 * @param v[in] vector applied to A, of size KYBER_K
 */
void polyvec_ntt_product(polyvec_t* r, const polyvec_t** A, const polyvec_t* v) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        polyvec_ntt_scalar_product(&r->vec[i], A[i], v);
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
 * @brief Replaces the square matrix A of order KYBER_K by its transpose
 * @param A
 */
void polyvec_transpose(polyvec_t** A) {
    int i, j;
    poly_t temp;

    for (i = 0; i < KYBER_K; i++) {
        for (j = i+1; j < KYBER_K; j++) {
            temp = A[i]->vec[j];
            A[i]->vec[j] = A[j]->vec[i];
            A[j]->vec[i] = temp;
        }
    }
}

/****************/
/* BYTES ENCODE */
/****************/

// REMPLACER LES 32 ETC PAR DES MACROS POLYVEC_TO_BYTES(d) ETC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/**
 * @brief Encodes a polyvec_t to a byte array of length 32 * d * KYBER_K, the encodings of the entries of the polyvec_t are concatenated in the output byte array
 * @param[out] bytes
 * @param[in] f
 * @param[in] d 
 */
void polyvec_byte_encode(uint8_t* bytes, const polyvec_t* f, const unsigned d) {
    int i;
    
    for (i = 0; i < KYBER_K; i++) {
        byte_encode(bytes + 32*d*i, f->vec[i].coeffs, d);
    }
}

/**
 * @brief Decodes a byte array of length 32 * d * KYBER_K to a polyvec_t, the decodings of the contiguous 32 bytes blocks of the byte array are the entries of the polyvec_t output
 * @param[out] f
 * @param[in] bytes
 * @param[in] d 
 */
void polyvec_byte_decode(polyvec_t* f, const uint8_t* bytes, const unsigned d) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        byte_decode(f->vec[i].coeffs, bytes + 32*d*i, d);
    }
}

/*********************************/
/* COMPRESSION AND DECOMPRESSION */
/*********************************/

/**
 * @brief Compresses all the coefficients of the entries of f
 * @param f 
 */
void polyvec_compress(polyvec_t* f, const unsigned d) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        f->vec[i] = compress(&f->vec[i], d);
    }
}

/**
 * @brief Decompresses all the coefficients of the entries of f
 * @param f 
 */
void polyvec_decompress(polyvec_t* f, const unsigned d) {
    int i;

    for (i = 0; i < KYBER_K; i++) {
        f->vec[i] = poly_decompress(&f->vec[i], d);
    }
}