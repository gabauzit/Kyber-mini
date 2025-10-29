/**
 * @file test_ntt.c
 * @details Test NTT functionalities
 * @author Gabriel Abauzit
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "consts.h"
#include "reduce.h"
#include "poly.h"
#include "ntt.h"

#ifndef NUM_TRIALS
	#define NUM_TRIALS 1000
#endif

poly_t random_poly() {
	int i;
	poly_t f;
	
	for (i = 0; i < KYBER_N; i++) {
		f.coeffs[i] = (int16_t)(rand() % KYBER_Q);
	}
	poly_reduce(&f);

	return f;
}

/*************************/
/* TESTS ON LIMIT VALUES */
/*************************/

// TEST 1 : NTT(0) = 0;

int test_NTT_zero() {
	poly_t f, g;
	
	poly_zero(&f);
	poly_zero(&g);

	NTT(f.coeffs);

	return poly_equal(&f, &g);
}

// TEST 2 : NTT_inv(0) = 0

int test_NTTinv_zero() {
	poly_t f, g;

	poly_zero(&f);
	poly_zero(&g);

	NTT_inv(f.coeffs);

	return poly_equal(&f, &g);
}

/*****************/
/* GENERIC TESTS */
/*****************/

// TEST 3 : NTT(NTT_inv(f)) = f

int test_NTT_NTTinv() {
	poly_t f = random_poly();
	poly_t g;

	poly_copy(&g, &f);
	
	// No need to convert into Montgomery domain since f, g are chosen at random
	NTT(f.coeffs);
	NTT_inv(f.coeffs);

	return poly_equal(&f, &g);
}

// TEST 4 : NTT(a + b) = NTT(a) + NTT(b)

int test_NTT_addition() {
	poly_t a = random_poly();
	poly_t b = random_poly();
	poly_t sum, sum_NTT; // sum = a + b, sum_NTT = NTT_inv(NTT(a) + NTT(b)), sum == sum_NTT is equivalent to NTT(a + b) == NTT(a) + NTT(b) according to TEST 1

	poly_add(&sum, &a, &b);

	NTT(a.coeffs);
	NTT(b.coeffs);
	poly_add(&sum_NTT, &a, &b);
	NTT_inv(sum_NTT.coeffs);

	return poly_equal(&sum, &sum_NTT);
}

// TEST 5 : NTT(a * b) = NTT(a) * NTT(b)

int test_NTT_multiplication() {
	/*
	poly_t a = random_poly();
    poly_t b = random_poly();
    poly_t prod_naive;
    poly_t prod_ntt;
    int32_t tmp[2 * KYBER_N - 1] = {0};

    // Produit naïf négacyclique
    for (int i = 0; i < KYBER_N; i++)
        for (int j = 0; j < KYBER_N; j++)
            tmp[i + j] += (int32_t)a.coeffs[i] * b.coeffs[j];

    for (int i = KYBER_N; i < 2 * KYBER_N - 1; i++)
        tmp[i - KYBER_N] -= tmp[i];

    for (int i = 0; i < KYBER_N; i++)
        prod_naive.coeffs[i] = barrett_reduce(tmp[i]);

    // Produit via NTT
    poly_mult(&prod_ntt, &a, &b);

    // Comparaison avec poly_equal
    return poly_equal(&prod_naive, &prod_ntt);
	*/
	poly_t a = random_poly();
    poly_t b = random_poly();
    poly_t prod_naif;
    poly_t prod_ntt;
	int temp[512] = {0};
	int i, j;

	poly_mult(&prod_ntt, &a, &b);

	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			temp[i+j] += (int)a.coeffs[i] * (int)b.coeffs[j];
		}
	}

	for (i = 0; i < 256; i++) {
		prod_naif.coeffs[i] = (int16_t)((temp[i] - temp[256+i]) % KYBER_Q);
		prod_naif.coeffs[i] = barrett_reduce(prod_naif.coeffs[i]);
	}

	return poly_equal(&prod_naif, &prod_ntt);
}

/**********************/
/* DISPLAYING RESULTS */
/**********************/

void display_results(int num, int success, int* test_success) {
	if (success == EXIT_SUCCESS) {
		(*test_success)++;
		printf("✅ TEST %i : Successful\n", num);
	}
	else {
		printf("❌ TEST %i : Failure\n", num);
	}
}

/********************/
/* TESTS EXECUTIONS */
/********************/

int main() {

	int test_success = 0;
	int test_total = 0;

	printf("╔══════════════════════════════════════╗\n");
	printf("║     RUNNING KYBER-mini NTT TESTS     ║\n");
	printf("╚══════════════════════════════════════╝\n");

	int i;
	int success;

	// TEST 1

	display_results(1, test_NTT_zero(), &test_success);
	test_total++;

	// TEST 2

	display_results(2, test_NTTinv_zero(), &test_success);
	test_total++;


	// TEST 3

	success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_NTT_NTTinv() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(3, success, &test_success);
	test_total++;

	// TEST 4

	success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_NTT_addition() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(4, success, &test_success);
	test_total++;

	// TEST 5

	success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_NTT_multiplication() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(5, success, &test_success);
	test_total++;

	/*****************/
	/* FINAL SUMMARY */
	/*****************/

	printf("╔════════════════════════════════════════╗\n");
	printf("║              FINAL SUMMARY             ║\n");
	printf("╚════════════════════════════════════════╝\n");
	printf("Successful tests : %i/%i\n", test_success, test_total);

	if (test_success == test_total) {
		printf("🎉 ALL TESTS WERE SUCCESSFUL 🎉\n");
		return EXIT_SUCCESS;
	}
	else {
		printf("⚠️  %i TEST(S) FAILED.\n", test_total - test_success);
		return EXIT_FAILURE;
	}

}