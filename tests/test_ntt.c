#include <stdio.h>
#include <stdlib.h>
#include "consts.h"
#include "reduce.h"
#include "poly.h"
#include "ntt.h"

#ifndef NBRE_ESSAIS
	#define NBRE_ESSAIS 1000
#endif

#define SUIVI_ESSAIS ((NBRE_ESSAIS >= 100) ? (NBRE_ESSAIS/100) : 1) // Pour le suivi des essais en terme de pourcentages

poly_t random_poly() {
	int i;
	poly_t f;
	
	for (i = 0; i < KYBER_N; i++) {
		f.coeffs[i] = (int16_t)(rand() % KYBER_Q); // Inutile d'avoir une réduction cryptographique ici
	}

	return f;
}

/********************/
/* TESTS GENERIQUES */
/********************/


// TEST 1 : Vérifie que NTT(f) renvoie un polynôme dans [0,q-1] si f l'est également A ENLEVER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int test_NTT_valid() {
	poly_t f = random_poly();
	NTT_inv(f.coeffs);
	poly_from_montgomery(&f);

	return poly_is_valid(&f);
}

// TEST 2 : NTT(NTT_inv(f)) = f

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

// TEST 3 : NTT(a + b) = NTT(a) + NTT(b)

int test_NTT_addition() {
	poly_t a = random_poly();
	poly_t b = random_poly();
	poly_t sum, sum_NTT; // sum = a + b. sum_NTT = NTT_inv(NTT(a) + NTT(b)), sum == sum_NTT équivaut à NTT(a + b) == NTT(a) + NTT(b) d'après le TEST 1

	poly_add(&sum, &a, &b);

	//poly_to_montgomery(&a);
	//poly_to_montgomery(&b);

	NTT(a.coeffs);
	NTT(b.coeffs);
	poly_add(&sum_NTT, &a, &b);
	NTT_inv(sum_NTT.coeffs);
		
	//poly_from_montgomery(&sum_NTT);

	return poly_equal(&sum, &sum_NTT);
}

// TEST 4 : NTT(a * b) = NTT(a) * NTT(b)

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
        tmp[i - KYBER_N] -= tmp[i];  // car x^256 ≡ -1 mod (x^256 + 1)
    }

	for (i = 0; i < KYBER_N; i++) {
        prod_naif.coeffs[i] = barrett_reduce(tmp[i]);
    }

	return poly_equal(&prod_naif, &prod_NTT);
}

/*********************************/
/* TESTS SUR DES VALEURS LIMITES */
/*********************************/

// TEST 5 : NTT(0) = 0;

int test_NTT_zero() {
	poly_t f, g;
	
	poly_zero(&f);
	poly_zero(&g);

	NTT(f.coeffs);

	return poly_equal(&f, &g); // Peu importe qu'on soit dans le domaine de Montgomery ou non, on doit toujours trouver 0
}

// TEST 6 : NTT_inv(0) = 0

int test_NTTinv_zero() {
	poly_t f, g;

	poly_zero(&f);
	poly_zero(&g);

	NTT_inv(f.coeffs);

	return poly_equal(&f, &g); // Peu importe qu'on soit dans le domaine de Montgomery ou non, on doit toujours trouver 0
}

// TEST 7 : NTT(NTT_inv(f)) = f pour f un monôme

int test_NTT_monome() {
	int i;
	int16_t c;
	poly_t f, g;

	i = rand() % KYBER_N;
	c = rand() % KYBER_Q;

	poly_zero(&f);
	f.coeffs[i] = c;
	poly_copy(&g, &f);

	//poly_to_montgomery(&f);
	NTT(f.coeffs);
	NTT_inv(f.coeffs);
	//poly_from_montgomery(&f);

	return poly_equal(&f, &g);
}

/**************************/
/* AFFICHAGE DE RESULTATS */
/**************************/

void afficher_resultat(int num, int success, int* test_success) {
	if (success) {
		(*test_success)++;
		printf("✅ TEST %i : RÉUSSI\n", num);
	}
	else {
		printf("❌ TEST %i : ÉCHEC\n", num);
	}
}


/***********************/
/* EXECUTION DES TESTS */
/***********************/

int main() {

	int test_success = 0;
	int test_total = 0;

	printf("╔════════════════════════════════════════════╗\n");
	printf("║     BATTERIE DE TESTS KYBER-mini - NTT     ║\n");
	printf("║     Implémentation: Gabriel Abauzit        ║\n");
	printf("╚════════════════════════════════════════════╝\n");

	int i;
	int success;

	// TEST 1

	success = 1;

	for (i = 0; i < NBRE_ESSAIS; i++) {
		success &= test_NTT_valid();
	}

	afficher_resultat(1, success, &test_success);
	test_total++;

	// TEST 2

	success = 1;

	for (i = 0; i < NBRE_ESSAIS; i++) {
		success &= test_NTT_NTTinv();
	}

	afficher_resultat(2, success, &test_success);
	test_total++;

	// TEST 3

	success = 1;

	for (i = 0; i < NBRE_ESSAIS; i++) {
		success &= test_NTT_addition();
	}

	afficher_resultat(3, success, &test_success);
	test_total++;

	// TEST 4

	success = 1;

	for (i = 0; i < NBRE_ESSAIS; i++) {
		success &= test_NTT_multiplication();
	}

	afficher_resultat(4, success, &test_success);
	test_total++;

	// TEST 5

	afficher_resultat(5, test_NTT_zero(), &test_success);
	test_total++;

	// TEST 6

	afficher_resultat(6, test_NTTinv_zero(), &test_success);
	test_total++;

	// TEST 7

	success = 1;

	for (i = 0; i < NBRE_ESSAIS; i++) {
		success &= test_NTT_monome();
	}

	afficher_resultat(7, success, &test_success);
	test_total++;
	
	// Résumé final

	printf("\n╔════════════════════════════════════════╗\n");
	printf("║              RÉSUMÉ FINAL              ║\n");
	printf("╚════════════════════════════════════════╝\n");
	printf("Tests réussis: %i/%i\n", test_success, test_total);

	if (test_success == test_total) {
		printf("\n🎉 TOUS LES TESTS SONT PASSÉS ! 🎉\n");
		return EXIT_SUCCESS;
	}
	else {
		printf("\n⚠️  %i test(s) ont échoué.\n", test_total - test_success);
		return EXIT_FAILURE;
	}

	return 0;
}