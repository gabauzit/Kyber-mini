/**
 * @file compress.h
 * @brief Compression and bytes conversion algorithms
 * @author Gabriel Abauzit
 */

#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>
#include "reduce.h"

/**************************/
/* BITS-BYTES CONVERSIONS */
/**************************/

void bits_to_bytes(uint8_t* bytes, const uint8_t* bits, unsigned l);

void bytes_to_bits(uint8_t* bits, const uint8_t* bytes, unsigned l);

/****************/
/* BYTES ENCODE */
/****************/

void byte_encode(uint8_t* bytes, const int16_t* F, const unsigned d);

void byte_decode(int16_t* F, const uint8_t* bytes, const unsigned d);

/*********************************/
/* COMPRESSION AND DECOMPRESSION */
/*********************************/

int16_t compress(const int16_t x, const unsigned d);

int16_t decompress(const int16_t x, const unsigned d);

#endif