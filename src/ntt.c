/**
 * @file ntt.h
 * @brief Implementation of the NTT related algorithms
 * @author Gabriel Abauzit
 */

#include "ntt.h"

/***************************************************************************************************/
/* The zeta tables are taken from FIPS 203 Appendix A, and then converted to the Montgomery domain */
/***************************************************************************************************/

// zetas[i] = zeta^{BitRev_7(i)} where zeta = 17 (mod 3329) is a primitive 256-th root of unity
const int16_t zetas[128] = {-1044, -758, -359, -1517, 1493, 1422, 287, 202, -171, 622, 1577, 182, 962, -1202, -1474, 1468, 573, -1325, 264, 383, -829, 1458, -1602, -130, -681, 1017, 732, 608, -1542, 411, -205, -1571, 1223, 652, -552, 1015, -1293, 1491, -282, -1544, 516, -8, -320, -666, -1618, -1162, 126, 1469, -853, -90, -271, 830, 107, -1421, -247, -951, -398, 961, -1508, -725, 448, -1065, 677, -1275, -1103, 430, 555, 843, -1251, 871, 1550, 105, 422, 587, 177, -235, -291, -460, 1574, 1653, -246, 778, 1159, -147, -777, 1483, -602, 1119, -1590, 644, -872, 349, 418, 329, -156, -75, 817, 1097, 603, 610, 1322, -1285, -1465, 384, -1215, -136, 1218, -1335, -874, 220, -1187, -1659, -1185, -1530, -1278, 794, -1510, -854, -870, 478, -108, -308, 996, 991, 958, -1460, 1522, 1628};

// zetas_basemul[i] = zeta{2*BitRev_7(i) + 1} with zeta = 17 (mod 3329)
const int16_t zetas_basemul[128] = {-1103, 1103, 430, -430, 555, -555, 843, -843, -1251, 1251, 871, -871, 1550, -1550, 105, -105, 422, -422, 587, -587, 177, -177, -235, 235, -291, 291, -460, 460, 1574, -1574, 1653, -1653, -246, 246, 778, -778, 1159, -1159, -147, 147, -777, 777, 1483, -1483, -602, 602, 1119, -1119, -1590, 1590, 644, -644, -872, 872, 349, -349, 418, -418, 329, -329, -156, 156, -75, 75, 817, -817, 1097, -1097, 603, -603, 610, -610, 1322, -1322, -1285, 1285, -1465, 1465, 384, -384, -1215, 1215, -136, 136, 1218, -1218, -1335, 1335, -874, 874, 220, -220, -1187, 1187, -1659, 1659, -1185, 1185, -1530, 1530, -1278, 1278, 794, -794, -1510, 1510, -854, 854, -870, 870, 478, -478, -108, 108, -308, 308, 996, -996, 991, -991, 958, -958, -1460, 1460, 1522, -1522, 1628, -1628};

/**
 * @brief Sens an array to its NTT transform
 * @details FIPS 203 Algorithm 9
 */
void NTT(int16_t tab[256]) {
    int len, start, i, j;
    int16_t zeta, t;

    i = 1;

    for (len = 128; len >= 2; len >>= 1) {
        for (start = 0; start < 256; start += 2*len) {
            zeta = zetas[i++];
            for (j = start; j < start + len; j++) {
                t = fqmul(zeta, tab[j + len]); 
                tab[j + len] = barrett_reduce(tab[j] - t);
                tab[j] = barrett_reduce(tab[j] + t);
            }
        }
    }
}

/**
 * @brief Sends an array to its inverse NTT transform
 * @details FIPS 203 Algorithm 10
 */
void NTT_inv(int16_t tab[256]) {
    int len, start, i, j;
    int16_t zeta, t;

    i = 127;

    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < 256; start += 2*len) {
            zeta = zetas[i--];
            for (j = start; j < start + len; j++) {
                t = tab[j];
                tab[j] = barrett_reduce(t + tab[j + len]);
                tab[j + len] = fqmul(zeta, tab[j + len] - t);
            }
        }
    }

    for (j = 0; j < 256; j++) {
        /*
         * The inverse NTT need a final normalization which consists of multiplying by 128^{-1} = 3303 (mod q).
         * Since we chose to do all the NTT computations inside the Montgomery domain, we shall also convert
         * the above constant into the Montgomery domain. This means replacing it by 128^{-1} * 2^16 = 512 (mod q).
         */
        tab[j] = fqmul(tab[j], 512);
    }
}

/**
 * @brief Multiplies two polynomials of degree 1 modulo a degree 2 polynomial
 * @details FIPS 203 Algorithm 12
 * 
 * @param r_0[out] degree 0 coefficient of the result
 * @param r_1[out] degree 1 coefficient of the result
 * @param a_0[in] degree 0 coefficient of the first polynomial
 * @param a_1[in] degree 1 coefficient of the first polynomial
 * @param b_0[in] degree 0 coefficient of the second polynomial
 * @param b_1[in] degree 1 coefficient of the second polynomial
 * @param m[in] the product is computed modulo x^2 - m (should come from the table zetas_basemul)
 */
void BaseCaseMultiply(int16_t* r0, int16_t* r1, const int16_t* a0, const int16_t* a1, const int16_t* b0, const int16_t* b1, const int16_t* m) {

    *r0 = fqmul(*a1, *b1);
    *r0 = fqmul(*r0, *m);
    *r0 += fqmul(*a0, *b0);
    *r1 = fqmul(*a0, *b1) + fqmul(*a1, *b0);
}

/**
 * @brief Multiplies two NTT together
 * @details Algorithm 11 FIPS 203
 */
void NTT_multiply(int16_t r[256], const int16_t a[256], const int16_t b[256]) {
    int i = 0;
    int16_t r0;
    int16_t r1;

    for (i = 0; i < 128; i++) {
        BaseCaseMultiply(&r0, &r1, &a[2*i], &a[2*i + 1], &b[2*i], &b[2*i + 1], &zetas_basemul[i]);
        r[2*i] = r0;
        r[2*i + 1] = r1;
    }
}