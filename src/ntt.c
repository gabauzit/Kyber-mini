#include "poly.h"


/* Les calculs dans la NTT ayant lieu dans le domaine de Montgomery, les tableaux zetas, zetas_inv et zetas_basemul sont précompilés dans ce dernier
 * pour éviter des conversions répétées pendant les calculs.
 * 
 * Tableaux obtenus à partir de l'Appendix A du FIPS-203, puis convertis dans le domaine de Montgomery à l'aide du programme Python ~/tables/zetas.py
 * 
 */

// zetas[i] = zeta^BitRev_7(i) dans le domaine de Montgomery
static const int16_t zetas[128] = {
    -1044, -758, -359, -1517, 1493, 1422, 287, 202,
    -171, 622, 1577, 182, 962, -1202, -1474, 1468,
    573, -1325, 264, 383, -829, 1458, -1602, -130,
    -681, 1017, 732, 608, -1542, 411, -205, -1571,
    1223, 652, -552, 1015, -1293, 1491, -282, -1544,
    516, -8, -320, -666, -1618, -1162, 126, 1469,
    -853, -90, -271, 830, 107, -1421, -247, -951,
    -398, 961, -1508, 1355, 1063, -1325, -221, 174,
    -1716, -475, 1335, -1181, -1464, 1013, -1532, -1606,
    -761, 1058, 1764, -470, -543, 1117, 736, -529,
    -1684, 1157, -1096, 488, 1001, -1163, -854, -1077,
    -1068, -1071, 1259, 1269, -1491, -1249, -1617, 1055,
    1180, -1044, -1185, 886, -512, -1611, -1097, 1064,
    1088, -1320, -1346, 322, 1138, -1464, 1019, 1015,
    1292, -1326, 851, 1117, -1618, -1297, -276, -1212,
    -296, 1448, 1081, -190, 542, 1606, -1297, 1203
};

// zetas_inv[i] = zeta^{-BitRev_7(i)} dans le domaine de Montgomery
static const int16_t zetas_inv[128] = {
    1701, 1807, 1460, 2371, 2338, 2333, 308, 108,
    2851, 870, 854, 1510, 2535, 1278, 1530, 1185,
    1659, 1187, 3109, 874, 1335, 2111, 136, 1215,
    2945, 1465, 1285, 2007, 2719, 2726, 2232, 2512,
    75, 156, 3000, 2911, 2980, 872, 2685, 1590,
    2210, 602, 1846, 777, 147, 2170, 2551, 246,
    1676, 1755, 460, 291, 235, 3152, 2742, 2907,
    3224, 1779, 2458, 1251, 2486, 2774, 2899, 1103,
    1275, 2652, 1065, 2881, 725, 1508, 2368, 398,
    951, 247, 1421, 3222, 2499, 271, 90, 853,
    1860, 3203, 1162, 1618, 666, 320, 8, -516,
    1544, 282, -1491, 1293, -1015, 552, -652, -1223,
    1571, 205, -411, 1542, -608, -732, -1017, 681,
    130, 1602, -1458, 829, -383, -264, 1325, -573,
    -1468, 1474, 1202, -962, -182, -1577, -622, 171,
    -202, -287, -1422, -1493, 1517, 359, 758, 1044
}; 

// zetas_basemul[i] = zeta^(2*BitRev7(i)+1) dans le domaine de Montgomery

const int16_t zetas_basemul[64] = {
    -758, 622, -1325, 1017, 652, -8, -90, 961,
    -475, 1058, 1157, -1071, -1044, -1320, -1326, 1448,
    -1517, 182, 383, 608, 1015, -320, -271, -1508,
    -1181, 1764, 488, 1259, 886, 1346, 851, 1081,
    1493, 962, -829, -1542, -1293, -666, 107, 1355,
    -1464, -470, 1001, 1269, -512, 322, 1117, 542,
    1422, -1202, 1458, 411, 1491, -1162, -1421, 1063,
    1013, -543, -1163, -1249, -1611, 1138, -1297, 1606
};

/*****************************/
/* CONVERSIONS DE MONTGOMERY */
/*****************************/

/*
 * fqmul(a,R^2) = (a*R^2)*R^{-1}=a*R, on envoie donc a dans le domaine de Montgomery
 * à appeler AVANT de d'effectuer une NTT
 *
 */

void poly_to_montgomery(poly_t* f) {
    int i;
    int16_t temp = 1353; //(2^16)^2 = 1353 (mod 3329)

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = fqmul(f->coeffs[i], temp);
    }
}

void poly_from_montgomery(poly_t* f) {
    int i;

    for (i = 0; i < KYBER_N; i++) {
        f->coeffs[i] = fqmul(f->coeffs[i], 1);
    }
}

/***********************/
/* TRANSFORMATIONS NTT */
/***********************/

// Algorithme 9 du FIPS-203

void NTT(poly_t* f) {
    unsigned len, start, i, j;
    int16_t zeta, t;

    i = 1;

    for (len = 128; len >= 2; len >>= 1) {
        for (start = 0; start < KYBER_N; start += len << 1) {
            zeta = zetas[i++];
            for (j = start; j < start + len; j++) {
                t = fqmul(f->coeffs[j + len], zeta);
                f->coeffs[j + len] = barrett_reduce(f->coeffs[j] - t);
                f->coeffs[j] = barrett_reduce(f->coeffs[j] + t);
            }
        }
    }
}

// Algorithme 10 du FIPS-203

void NTT_inv(poly_t* f) {
    unsigned int len, start, i, j;
    int16_t zeta, t;

    i = 127;

    for (len = 2; len <= 128; len <<= 1) {
        for (start = 0; start < KYBER_N; start += len << 1) {
            zeta = zetas_inv[i--];
            for (j = start; j < start + len; j++) {
                t = f->coeffs[j];
                f->coeffs[j] = barrett_reduce(t + f->coeffs[j + len]);
                f->coeffs[j + len] = barrett_reduce(t - f->coeffs[j + len]);
                f->coeffs[j + len] = fqmul(zeta, f->coeffs[j + len]);
            }
        }
    }

    for (j = 0; j < KYBER_N; j++) {
        /*
        * La NTT inverse nécessite une normalisation finale consistant à diviser par 256.
        * Or, si on veut rester dans le domaine de Montgomery, on aimerait en réalité obtenir au final
        * f[j] * 256^{-1} * 2^16 = fqmul(f[j], 256^{-1} * (2^16)^2)
        * et 256^{-1} * (2^16)^2 = 1441 (mod 3329)
        * 
        */
        f->coeffs[j] = fqmul(f->coeffs[j], 1441); 
    }
}

/**********************************/
/* OPERATIONS DANS LE DOMAINE NTT */
/**********************************/

// Algorithme 12 du FIPS-203

void BaseCaseMultiply(int16_t* r0, int16_t* r1, const int16_t* a0, const int16_t* a1, const int16_t* b0, const int16_t* b1, const int16_t *m) {

    *r0 = fqmul(*a1, *b1);
    *r0 = fqmul(*r0, *m);
    *r0 = barrett_reduce(*r0 + fqmul(*a0, *b0));
    *r1 = barrett_reduce(fqmul(*a0, *b1) + fqmul(*a1, *b0));
}

// Algorithme 11 du FIPS-203

void MultiplyNTT(poly_t* r, const poly_t* a, const poly_t* b) {
    int i = 0;
    int16_t r0;
    int16_t r1;

    for (i = 0; i < KYBER_N / 2; i++) {
        BaseCaseMultiply(&r0, &r1, &a->coeffs[2 * i], &a->coeffs[2 * i + 1], &b->coeffs[2 * i], &b->coeffs[2 * i + 1], &zetas_basemul[i]);
        r->coeffs[2 * i] = r0;
        r->coeffs[2 * i + 1] = r1;
    }
}


/**********************************/
/* MULTIPLICATION RAPIDE AVEC NTT */
/**********************************/

void NTT_mult(poly_t* r, poly_t* a, poly_t* b) {

    poly_to_montgomery(a);
    poly_to_montgomery(b);

    NTT(a);
    NTT(b);

    MultiplyNTT(r, a, b);
    NTT_inv(r);
    poly_from_montgomery(r);
}