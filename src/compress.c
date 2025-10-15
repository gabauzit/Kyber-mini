#include <stdlib.h>
#include <stdint.h>
#include "poly.c"

/***************/
/* COMPRESSION */
/***************/

uint16_t compress(int16_t a, int d) {
	uint32_t temp;

	temp = barrett_reduce(a);
	temp = (uint32_t)x << d;
	temp = (temp + KYBER_Q / 2) / KYBER_Q;	
	return temp & ((1 << d) - 1); //ne garde que les d bits de poids faibles de temp
}

void poly_compress(poly* r, int d) {
	unsigned i;

	for (i = 0; i < KYBER_N; i++) {
		r->coeffs[i] = compress(r->coeffs[i], d);
	}
}

/*****************/
/* DECOMPRESSION */
/*****************/

uint16_t decompress(int16_t a, int d) {
	uint32_t temp;

	temp = (uint32_t)a * KYBER_Q;
	temp += (1 << (d - 1));
	return temp >> d;
}

void poly_decompress(poly* r, int d) {
	unsigned int i;
	for (i = 0; i < KYBER_N; i++) {
		r->coeffs[i] = decompress(r->coeffs[i], d);
	}
}