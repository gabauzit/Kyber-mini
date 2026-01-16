/**
 * @file consts.h
 * @brief Constants and Kyber parameters
 * @author Gabriel Abauzit
 *
 */

#ifndef CONSTS_H
#define CONSTS_H

/*****************************/
/* KYBER CONSTANT PARAMETERS */
/*****************************/

#define KYBER_N 256
#define KYBER_Q 3329

 /*************************/
 /* ML-KEM-512 PARAMETRES */
 /*************************/

#define KYBER_K 2
#define KYBER_ETA1 3
#define KYBER_ETA2 2
#define KYBER_DU 10
#define KYBER_DV 4

/*****************/
/* NTT CONSTANTS */
/*****************/

#define ZETA 17         // Racine primtiive 256-ème de l'unité dans Z_q

#define MONTGOMERY_R 2285 // 2^16 = 2285 mod 3329
#define MONTGOMERY_RINV 169 // (2^16)^{-1} = 169 mod 3329
#define MONTGOMERY_INV 3327 // -q^-1 mod 2^16
#define MONTGOMERY_QINV 62209 // q^-1 mod 2^16 */

#define BARRETT_FACTOR 20159 // nearest integer to 2^26/q  ((1<<26) + KYBER_Q/2)/KYBER_Q;

#endif