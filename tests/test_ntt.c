#include <stdio.h>
#include <stdlib.h>
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

/*****************/
/* GENERIC TESTS */
/*****************/


// TEST 1 : NTT(NTT_inv(f)) = f

int test_NTT_NTTinv() {
	poly_t f = random_poly();
	poly_t g;

	poly_reduce(&f);
	poly_copy(&g, &f);
	
	//poly_to_montgomery(&f);
	NTT(f.coeffs);
	NTT_inv(f.coeffs);
	//poly_from_montgomery(&f);

	return poly_equal(&f, &g);
}

// TEST 2 : NTT(a + b) = NTT(a) + NTT(b)

int test_NTT_addition() {
	poly_t a = random_poly();
	poly_t b = random_poly();
	poly_t sum, sum_NTT; // sum = a + b. sum_NTT = NTT_inv(NTT(a) + NTT(b)), sum == sum_NTT is equivalent to NTT(a + b) == NTT(a) + NTT(b) according to TEST 1

	poly_add(&sum, &a, &b);

	NTT(a.coeffs);
	NTT(b.coeffs);
	poly_add(&sum_NTT, &a, &b);
	NTT_inv(sum_NTT.coeffs);

	for (int k = 0; k < KYBER_N; k++) {
		if (sum.coeffs[k] != sum_NTT.coeffs[k]) {
			printf("degré %i : sum = %i, sum_NTT = %i\n", k, (int)sum.coeffs[k], (int)sum_NTT.coeffs[k]);
		}
	}

	return poly_equal(&sum, &sum_NTT);
}

// TEST 3 : NTT(a * b) = NTT(a) * NTT(b)

int test_NTT_multiplication() {
	int i, j;
	//int32_t temp[KYBER_N];
	int32_t tmp[2 * KYBER_N - 1] = {0};

	poly_t a = random_poly();
	poly_t b = random_poly();
	poly_t prod_naif; // prod_naif vaut a * b par convolution négacyclique
	poly_t prod_NTT; // prod_NTT = NTT_inv(NTT(a) * NTT(b)) et on vérifie de même si prod_naif == prod_NTT

	// Produit NTT
	
	poly_mult(&prod_NTT, &a, &b);

	// Produit naïf

	/*for (i = 0; i < KYBER_N; i++) {
		temp[i] = 0;
		for (j = 0; j <= i; j++) {
			temp[i] = barrett_reduce((int32_t)temp[i] + a.coeffs[j] * b.coeffs[i - j]);
		}
		for (j = i + 1; j < KYBER_N; j++) {
			temp[i] = barrett_reduce((int32_t)temp[i] - a.coeffs[j] * b.coeffs[KYBER_N + i - j]);
		}
	
		prod_naif.coeffs[i] = (int16_t)temp[i];
	}
	*/

	for (i = 0; i < KYBER_N; i++) {
        for (j = 0; j < KYBER_N; j++) {
            tmp[i + j] += (int32_t)a.coeffs[i] * b.coeffs[j];
        }
    }

	for (i = KYBER_N; i < 2 * KYBER_N - 1; i++) {
        tmp[i - KYBER_N] -= tmp[i];  // x^256 ≡ -1 mod (x^256 + 1)
    }

	for (i = 0; i < KYBER_N; i++) {
        prod_naif.coeffs[i] = barrett_reduce(tmp[i]);
    }

	return poly_equal(&prod_naif, &prod_NTT);
}

/*************************/
/* TESTS ON LIMIT VALUES */
/*************************/

// TEST 4 : NTT(0) = 0;

int test_NTT_zero() {
	poly_t f, g;
	
	poly_zero(&f);
	poly_zero(&g);

	NTT(f.coeffs);

	return poly_equal(&f, &g);
}

// TEST 5 : NTT_inv(0) = 0

int test_NTTinv_zero() {
	poly_t f, g;

	poly_zero(&f);
	poly_zero(&g);

	NTT_inv(f.coeffs);

	return poly_equal(&f, &g);
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

	success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_NTT_NTTinv() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(1, success, &test_success);
	test_total++;

	// TEST 2

	success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_NTT_addition() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(2, success, &test_success);
	test_total++;

	// TEST 3

	success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_NTT_multiplication() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(3, success, &test_success);
	test_total++;

	// TEST 4

	display_results(5, test_NTT_zero(), &test_success);
	test_total++;

	// TEST 5

	display_results(6, test_NTTinv_zero(), &test_success);
	test_total++;

/*****************/
/* FINAL SUMMARY */
/*****************/

	printf("╔════════════════════════════════════════╗\n");
	printf("║              FINAL SUMMARY             ║\n");
	printf("╚════════════════════════════════════════╝\n");
	printf("Successful tests : %i/%i\n", test_success, test_total);

	if (test_success == test_total) {
		printf("\n🎉 ALL TESTS WERE SUCCESSFUL 🎉\n");
		return EXIT_SUCCESS;
	}
	else {
		printf("\n⚠️  %i TEST(S) FAILED.\n", test_total - test_success);
		return EXIT_FAILURE;
	}
}