/**
 * @file test_encode.c
 * @details Test encode functionalities
 * @author Gabriel Abauzit
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "encode.h"

#ifndef NUM_TRIALS
	#define NUM_TRIALS 1000
#endif

#ifndef MAX_L
    #define MAX_L 100
#endif

void random_bytes(uint8_t* tab, unsigned l) {
    unsigned i;

    for (i = 0; i < l; i++) {
        tab[i] = (uint8_t)(rand() % 256);
    }
}

void random_bits(uint8_t*tab, unsigned l) {
    unsigned i;

    for (i = 0; i < l; i++) {
        tab[i] = (uint8_t)(rand() % 2);
    }
}

/**************************/
/* BITS-BYTES CONVERSIONS */
/**************************/

// TEST 1 : bytes_to_bits(bits_to_bytes(tab)) = tab

int test_bytes_to_bits_bits_to_bytes() {
    unsigned i;
    uint8_t diff = 0;
    unsigned l = (unsigned)(1 + rand() % MAX_L);
	uint8_t *tab, *tab_copy, *temp;
    
    tab = (uint8_t*)malloc(8*l);
    tab_copy = (uint8_t*)malloc(8*l);
    temp = (uint8_t*)malloc(l);
    
    random_bits(tab, 8*l);
    memcpy(tab_copy, tab, 8*l);
   
    bits_to_bytes(temp, tab, l);
    bytes_to_bits(tab, temp, l);

    for (i = 0; i < 8*l; i++) {
        diff |= tab[i] ^ tab_copy[i];
    }

    free(tab);
    free(tab_copy);
    free(temp);

    return diff == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

// TEST 2 : bits_to_bytes(bytes_to_bits(tab)) = tab

int test_bits_to_bytes_bytes_to_bits() {
    unsigned i;
    uint8_t diff = 0;
    unsigned l = (unsigned)(1 + rand() % MAX_L);
	uint8_t *tab, *tab_copy, *temp;
    
    tab = (uint8_t*)malloc(l);
    tab_copy = (uint8_t*)malloc(l);
    temp = (uint8_t*)malloc(8*l);
    
    random_bytes(tab, l);
    memcpy(tab_copy, tab, l);
   
    bytes_to_bits(temp, tab, l);
    bits_to_bytes(tab, temp, l);

    for (i = 0; i < l; i++) {
        diff |= tab[i] ^ tab_copy[i];
    }

    free(tab);
    free(tab_copy);
    free(temp);

    return diff == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*********************************/
/* COMPRESSION AND DECOMPRESSION */
/*********************************/

// TEST 3 : compress(decompress(compress(x))) = x

int test_compress_decompress_compress() {
    unsigned d = (unsigned)(rand() % 12);
    int16_t x = (int16_t)(rand() & 0xFFFF);
    int16_t x_compressed, temp;

    x_compressed = compress(x, d);
    temp = decompress(x_compressed, d);
    temp = compress(temp, d);

    return x_compressed == temp ? EXIT_SUCCESS : EXIT_FAILURE;
}

// TEST 4 : |x - decompress(compress(x))| <= q/2^(d+1), q = 3329

int test_decompress_compress() {
    int16_t q = 3329;
    unsigned d = 12;
    int16_t x = (int16_t)(rand() % q);
    int16_t temp, err, err_max;

    temp = compress(x, d);
    temp = decompress(temp, d);

    err = x > temp ? x - temp : temp - x;
    err_max = (q >> (d+1)) + 1;
    
    return err <= err_max ? EXIT_SUCCESS : EXIT_FAILURE;
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

	printf("╔═════════════════════════════════════════╗\n");
	printf("║      RUNNING KYBER-mini ENCODE TESTS    ║\n");
	printf("╚═════════════════════════════════════════╝\n");

    int i;
	int success;

	// TEST 1
    
    success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_bytes_to_bits_bits_to_bytes() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(1, success, &test_success);
	test_total++;

	// TEST 2

    success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_bits_to_bytes_bytes_to_bits() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(2, success, &test_success);
	test_total++;

    // TEST 3

    success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_compress_decompress_compress() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(3, success, &test_success);
	test_total++;

    // TEST 4

    success = EXIT_SUCCESS;

	for (i = 0; i < NUM_TRIALS; i++) {
		if (test_decompress_compress() == EXIT_FAILURE) {
			success = EXIT_FAILURE;
		}
	}

	display_results(4, success, &test_success);
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