/**
 * @file encoding.h
 * @brief Compression et Décompression
 * @author Gabriel Abauzit
 *
 */

#ifndef ENCODING_H
#define ENCODING_H

#include "poly.h"

/*********************/
/* ENCODAGE DANS Z_q */
/*********************/

/**
 * @brief Compression d'un élément de Z_q
 *
 * Maps x ∈ [0, q-1] to y ∈ [0, 2^d-1]
 * Formula: y = ⌈(2^d / q) * x⌋ mod 2^d
 *
 * @param x entier de Z_q à compresser
 * @param d bits de compression
 * @return compression de x
 *
 */

static inline uint16_t compress(int16_t x, int d) {
    // FAUT-IL VERIFIER QUE x EST EFFECTIVEMENT ENTRE 0 ET q-1 ??????
    uint32_t t;

    // t = (2^d * x + q/2) / q
    t = (uint32_t)x << d;
    t += KYBER_Q / 2;
    t /= KYBER_Q;

    return (uint16_t)(t & ((1 << d) - 1));
}

/**
 * @brief Décompresse un élément de Z_q
 *
 * Maps y ∈ [0, 2^d-1] to x ∈ [0, q-1]
 * Formula: x = ⌈(q / 2^d) * y⌋
 *
 * @param y Compressed value
 * @param d Compression bits
 * @return Decompressed coefficient
 *
 * Time complexity: O(1), constant-timex
 */

static inline int16_t decompress(uint16_t x, int d) {
    uint32_t t;

    x = barrett_reduce(x);

    /* Compute (q * y + 2^(d-1)) / 2^d */
    t = (uint32_t)x * KYBER_Q;
    t += ((uint32_t)1 << (d - 1));

    return (int16_t)(t >> d);
}

/*************************/
/* ENCODAGE DE POLYNOMES */
/*************************/


/**
 * @brief Compression d'un polynôme
 *
 * @param r polynôme à compresser
 * @param d bits de compression
 *
 */

void poly_compress(poly_t* f, int d);

/**
 * @brief Décompresse un polynôme
 *
 * @param r polynôme à décompresser
 * @param d bits de compression
 * 
 */

void poly_decompress(poly_t* f, int d);

/*********************/
/* CODAGE ET DECODAGE */
/*********************/

 /**
  * @brief Encode polynomial to bytes (Algorithm 5)
  *
  * Converts array of 256 d-bit integers to byte array
  * Output size: 32*d bytes
  *
  * @param bytes Output byte array
  * @param p Input polynomial
  * @param d Bits per coefficient (1 ≤ d ≤ 12)
  *
  * Time complexity: O(n), constant-time
  *
  * For d=12: encodes to 384 bytes (full precision)
  * For d=10: encodes to 320 bytes (compressed u)
  * For d=4:  encodes to 128 bytes (compressed v)
  */
void poly_to_bytes(uint8_t* bytes, const poly_t* f, int d);

/**
 * @brief Decode polynomial from bytes (Algorithm 6)
 *
 * Converts byte array to array of 256 d-bit integers
 * Input size: 32*d bytes
 *
 * @param p Output polynomial
 * @param bytes Input byte array
 * @param d Bits per coefficient (1 ≤ d ≤ 12)
 *
 * Time complexity: O(n), constant-time
 *
 * For d=12: decodes from 384 bytes (full precision)
 * For d=10: decodes from 320 bytes (compressed u)
 * For d=4:  decodes from 128 bytes (compressed v)
 */
void poly_from_bytes(poly_t* f, const uint8_t* bytes, int d);

/*************************************************
 * Bit manipulation utilities
 *************************************************/

 /**
  * @brief Convert bit array to byte array
  *
  * @param bytes Output byte array
  * @param bits Input bit array (length must be multiple of 8)
  * @param nbits Number of bits
  *
  * Time complexity: O(nbits), constant-time
  */
void bits_to_bytes(uint8_t* bytes, const uint8_t* bits, size_t nbits);

/**
 * @brief Convert byte array to bit array
 *
 * @param bits Output bit array
 * @param bytes Input byte array
 * @param nbytes Number of bytes
 *
 * Time complexity: O(nbytes), constant-time
 */
void bytes_to_bits(uint8_t* bits, const uint8_t* bytes, size_t nbytes);

#endif