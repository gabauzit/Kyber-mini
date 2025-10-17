/**
 * @file ntt.h
 * @brief Implementation of the NTT related algorithms
 * @author Gabriel Abauzit
 */

#include "ntt.h"

/* Les calculs dans la NTT ayant lieu dans le domaine de Montgomery, les tableaux zetas, zetas_inv et zetas_basemul sont précompilés dans ce dernier
 * pour éviter des conversions répétées pendant les calculs.
 *
 * Tableaux obtenus à partir de l'Appendix A du FIPS-203, puis convertis dans le domaine de Montgomery à l'aide du programme Python ~/tables/zetas.py
 *
 */

const int16_t zetas[128] = { -1044, -758, -359, -1517, 1493, 1422, 287, 202, -171, 622, 1577, 182, 962, -1202, -1474, 1468, 573, -1325, 264, 383, -829, 1458, -1602, -130, -681, 1017, 732, 608, -1542, 411, -205, -1571, 1223, 652, -552, 1015, -1293, 1491, -282, -1544, 516, -8, -320, -666, -1618, -1162, 126, 1469, -853, -90, -271, 830, 107, -1421, -247, -951, -398, 961, -1508, -725, 448, -1065, 677, -1275, -1103, 430, 555, 843, -1251, 871, 1550, 105, 422, 587, 177, -235, -291, -460, 1574, 1653, -246, 778, 1159, -147, -777, 1483, -602, 1119, -1590, 644, -872, 349, 418, 329, -156, -75, 817, 1097, 603, 610, 1322, -1285, -1465, 384, -1215, -136, 1218, -1335, -874, 220, -1187, -1659, -1185, -1530, -1278, 794, -1510, -854, -870, 478, -108, -308, 996, 991, 958, -1460, 1522, 1628 };


/*const int16_t zetas[128] = {1, 1729, 2580, 3289, 2642, 630, 1897, 848,
1062 ,1919 ,193 ,797 ,2786 ,3260 ,569 ,1746,
296, 2447, 1339 ,1476 ,3046, 56 ,2240, 1333,
1426 ,2094 ,535 ,2882 ,2393 ,2879 ,1974 ,821,
289 ,331 ,3253 ,1756, 1197, 2304 ,2277, 2055,
650, 1977, 2513, 632 ,2865 ,33 ,1320 ,1915,
2319, 1435 ,807 ,452 ,1438, 2868, 1534, 2402,
2647, 2617, 1481, 648 ,2474 ,3110, 1227, 910,
17 ,2761 ,583, 2649, 1637, 723, 2288, 1100,
1409 ,2662 ,3281 ,233 ,756 ,2156, 3015 ,3050,
1703, 1651, 2789, 1789, 1847, 952, 1461, 2687,
939 ,2308 ,2437, 2388, 733 ,2337, 268, 641,
1584, 2298, 2037, 3220, 375, 2549, 2090, 1645
,1063 ,319 ,2773 ,757 ,2099, 561, 2466, 2594,
2804, 1092, 403 ,1026, 1143, 2150, 2775, 886,
1722, 1212, 1874, 1029, 2110, 2935, 885, 2154 };*/ // DANS LE DOMAINE NORMALE

const int16_t zetas_basemul[128] = { 2226, 1103, 430, 2899, 555, 2774, 843, 2486, 2078, 1251, 871, 2458, 1550, 1779, 105, 3224, 422, 2907, 587, 2742, 177, 3152, 3094, 235, 3038, 291, 2869, 460, 1574, 1755, 1653, 1676, 3083, 246, 778, 2551, 1159, 2170, 3182, 147, 2552, 777, 1483, 1846, 2727, 602, 1119, 2210, 1739, 1590, 644, 2685, 2457, 872, 349, 2980, 418, 2911, 329, 3000, 3173, 156, 3254, 75, 817, 2512, 1097, 2232, 603, 2726, 610, 2719, 1322, 2007, 2044, 1285, 1864, 1465, 384, 2945, 2114, 1215, 3193, 136, 1218, 2111, 1994, 1335, 2455, 874, 220, 3109, 2142, 1187, 1670, 1659, 2144, 1185, 1799, 1530, 2051, 1278, 794, 2535, 1819, 1510, 2475, 854, 2459, 870, 478, 2851, 3221, 108, 3021, 308, 996, 2333, 991, 2338, 958, 2371, 1869, 1460, 1522, 1807, 1628, 1701 };

/*static const int16_t zetas_basemul[128] = { 17, -17 ,2761, -2761, 583, -583 ,2649 ,-2649,
1637 ,-1637 ,723 ,-723 ,2288 ,-2288 ,1100, -1100
,1409 ,-1409, 2662, -2662 ,3281 ,-3281, 233 ,-233,
756 ,-756, 2156, -2156 ,3015 ,-3015, 3050, -3050,
1703 ,-1703 ,1651, -1651, 2789 ,-2789, 1789, -1789,
1847 ,-1847 ,952, -952, 1461, -1461, 2687 ,-2687,
939, -939, 2308, -2308, 2437 ,-2437 ,2388 ,-2388,
733, -733 ,2337 ,-2337, 268, -268, 641 ,-641,
1584, -1584, 2298 ,-2298 ,2037 ,-2037, 3220, -3220,
375, -375, 2549 ,-2549 ,2090 ,-2090, 1645, -1645
,1063 ,-1063 ,319 ,-319, 2773, -2773, 757, -757,
2099 ,-2099, 561, -561, 2466, -2466, 2594 ,-2594,
2804, -2804, 1092, -1092, 403, -403, 1026 ,-1026,
1143, -1143, 2150 ,-2150 ,2775, -2775, 886, -886,
1722, -1722, 1212 ,-1212 ,1874 ,-1874, 1029, -1029,
2110, -2110, 2935, -2935, 885 ,-885 ,2154 ,-2154 }; */ //DANS LE DOMAINE NORMAL

/**
 * @brief Sens f to its NTT transform
 * @details FIPS 203 Algorithm 9
 */
void NTT(int16_t f[256]) {
    int len, start, i, j;
    int16_t zeta, t;

    i = 1;

    for (len = 128; len >= 2; len >>= 1) {
        for (start = 0; start < KYBER_N; start += 2*len) {
            zeta = zetas[i++];
            for (j = start; j < start + len; j++) {
                t = fqmul(zeta, f[j + len]); 
                f[j + len] = barrett_reduce(f[j] - t);
                f[j] = barrett_reduce(f[j] + t);
            }
        }
    }
}

/**
 * @brief Sends f to its inverse NTT transform
 * @details FIPS 203 Algorithm 10
 */
void NTT_inv(int16_t f[256]) {
    int len, start, i, j;
    int16_t zeta, t;

    i = 127;

    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < KYBER_N; start += 2*len) {
            zeta = zetas[i--];
            for (j = start; j < start + len; j++) {
                t = f[j];
                f[j] = barrett_reduce(t + f[j + len]);
                f[j + len] = fqmul(zeta, f[j + len] - t);
            }
        }
    }

    for (j = 0; j < KYBER_N; j++) {
        /*
         * The inverse NTT need a final normalization which consists of multiplying by 128^{-1} = 3303 (mod q).
         * Since we chose to do all the NTT computations inside the Montgomery domain, we shall also convert
         * the above constant into the Montgomery domain. This means replacing it by 128^{-1} * 2^16 = 512 (mod q).
         */

        f[j] = fqmul(f[j], 512);
        //f[j] = fqmul(f[j], 3303);
    }
}

/**
 * @brief Multiplies two polynomials of degree 1 modulo a degree 2 polynomial
 * @details Algorithm 12 of FIPS 203
 * @param r_0[out] degree 0 coefficient of the result
 * @param r_1[out] degree 1 coefficient of the result
 * @param a_0[in] degree 0 coefficient of the first polynomial
 * @param a_1[in] degree 1 coefficient of the first polynomial
 * @param b_0[in] degree 0 coefficient of the second polynomial
 * @param b_1[in] degree 1 coefficient of the second polynomial
 * @param m[in] the product is computed modulo x^2 - m (should come from the table zetas_basemul)
 *
 */
void BaseCaseMultiply(int16_t* r0, int16_t* r1, const int16_t* a0, const int16_t* a1, const int16_t* b0, const int16_t* b1, const int16_t* m) {

    *r0 = fqmul(*a1, *b1);
    *r0 = fqmul(*r0, *m);
    *r0 = barrett_reduce(*r0 + fqmul(*a0, *b0));
    *r1 = barrett_reduce(fqmul(*a0, *b1) + fqmul(*a1, *b0));
}

/**
 * @brief Multiplies two NTT together
 * @details FIPS 203 Algorithm 11
 * @param r[out] result
 * @param a[in] : first operand
 * @param b[in] : second operand
 */
void NTT_multiply(int16_t r[256], const int16_t a[256], const int16_t b[256]) {
    int i = 0;
    int16_t r0;
    int16_t r1;

    for (i = 0; i < KYBER_N / 2; i++) {
        BaseCaseMultiply(&r0, &r1, &a[2 * i], &a[2 * i + 1], &b[2 * i], &b[2 * i + 1], &zetas_basemul[i]);
        r[2 * i] = r0;
        r[2 * i + 1] = r1;
    }
}