#ifndef CONSTS_H
#define CONSTS_H

/**
 * @file consts.h
 * @brief Constantes de Kyber
 * @author Gabriel Abauzit
 *
 */

#include <stdint.h>
#include <stddef.h>


#define KYBER_N 256
#define KYBER_Q 3329

/**************/
/* ML-KEM-512 */
/**************/

#define KYBER_K 2
#define KYBER_ETA1 3
#define KYBER_ETA2 2
#define KYBER_DU 10
#define KYBER_DV 4

/************************/
/* Constantes de la NTT */
/************************/

#define ZETA 17         // Racine primtiive 256-ème de l'unité dans Z_q

#define MONTGOMERY_R 2285 // 2^16 = 2285 mod 3329
#define MONTGOMERY_RINV 169 // (2^16)^{-1} = 169 mod 3329
#define MONTGOMERY_INV 3327 // -q^-1 mod 2^16
#define MNTGOMERY_QINV 62209 // q^-1 mod 2^16 */

#define BARRETT_FACTOR 20159

#endif