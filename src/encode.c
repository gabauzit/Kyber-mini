/**
 * @file compress.c
 * @brief Compression and bytes conversion algorithms
 * @author Gabriel Abauzit
 */

#include "encode.h"

/**************************/
/* BITS-BYTES CONVERSIONS */
/**************************/

/**
 * @brief Converts a bit array into an array of bytes
 * @details FIPS 203 Algorithm 3
 * 
 * @param[out] bytes bytes array of size l
 * @param[in] bits bits array of size 8 * l
 * @param l
 */
void bits_to_bytes(uint8_t* bytes, const uint8_t* bits, unsigned l) {
    int i, j;

    for (i = 0; i < l; i++) {
        bytes[i] = 0;
    }

    for (i = 0; i < l; i++) {
        for (j = 0; j < 8; j++) {
            bytes[i] |= bits[8*i + j] << j;
        }
    }
}

/**
 * @brief Converts a bytes array into an array of bits
 * @details FIPS 203 Algorithm 4
 * 
 * @param[out] bits bits array of size 8 * l
 * @param[in] bytes bytes array of size l
 * @param l
 */
void bytes_to_bits(uint8_t* bits, const uint8_t* bytes, unsigned l) {
    int i, j;
    
    for (i = 0; i < l; i++) {
        for (j = 0; j < 8; j++) {
            bits[8*i + j] = (bytes[i] >> j) & 1;
        }
    }
}

/****************/
/* BYTES ENCODE */
/****************/

/**
 * @brief Encodes an array of integers into a byte array
 * @details FIPS 203 Algorithm 5
 * 
 * @param[out] bytes byte array of size 32 * d
 * @param[in] F int16_t array of size 256
 * @param[in] d should be between 1 and 12
 */
void byte_encode(uint8_t* bytes, const int16_t* F, const unsigned d) {
    int i, j;
    int16_t a;
    int8_t b[256 * d];
    int16_t lower_bits; // To reduce mod 2^m where m = 2^d if d < 12, m = 3329 otherwise

    // If d = 12, we can remove the mod 3329 reduction because this is already the case when working with Kyber. The operation &= lower_bits does nothing in this case.
    lower_bits = (1U << d) - 1;

    for (i = 0; i < 256; i++) {
        a = F[i] & lower_bits;
        for (j = 0; j < d; j++) {
            b[i*d + j] = a & 1;
            a >>= 1;
        }
    }

    bits_to_bytes(bytes, b, 32*d);
}

/**
 * @brief Encodes byte array into an array of integers
 * @details FIPS 203 Algorithm 6
 * 
 * @param[out] F int16_t array of size 256
 * @param[in] bytes byte array of size 32 * d
 * @param d should be between 1 and 12
 */
void byte_decode(int16_t* F, const uint8_t* bytes, const unsigned d) {
    int i, j;
    uint8_t* b;
    int16_t temp;
    int16_t lower_bits; // To reduce mod 2^m where m = 2^d if d < 12, m = 3329 otherwise

    // If d = 12, we can remove the mod 3329 reduction because this is already the case when working with Kyber. The operation &= lower_bits does nothing in this case.
    lower_bits = (1U << d) - 1;

    b = (uint8_t*)malloc(256 * d * sizeof(uint8_t));
    bytes_to_bits(b, bytes, 32*d);

    for (i = 0; i < 256; i++) {
        temp = 0;
        for (j = 0; j < d; j++) {
            temp |= b[i*d + j] << j;
        }
        F[i] = temp & lower_bits;
    }

    memset(b, 0, 256 * d * sizeof(uint8_t));
    free(b);
}

/*********************************/
/* COMPRESSION AND DECOMPRESSION */
/*********************************/

/**
 * @brief Returns the mod 2^d reduction of the integer closest to x * 2^d / q
 * @param x
 * @param d
 */
int16_t compress(const int16_t x, const unsigned d) {
    uint32_t t;

    t = ((int32_t)x << d) + (KYBER_Q / 2);
    t = (int32_t)(((int64_t)t * BARRETT_FACTOR) >> 26);
    t &= (1U << d) - 1;
    return (int16_t)t;
}

/**
 * @brief Returns the mod q reduction of the integer closest to x * q / 2^d
 * @param x
 * @param d 
 */
int16_t decompress(const int16_t x, const unsigned d) {
    uint32_t t;

    t = ((int32_t)x * KYBER_Q) + (1U << (d-1));
    t >>= d;
    return (int16_t)t;
}