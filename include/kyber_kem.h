/**
 * @file kyber_kem.h
 * @brief Key Exchange Mechanism part of Kyber
 * @author Gabriel Abauzit
 */

#ifndef KYBER_KEM_H
#define KYBER_KEM_H

#include <stdint.h>
#include "consts.h"
#include "kyber_pke.h"
#include "wrapper.h"

/**************************************************************************************/
/* Internal algorithms are determinstic, they should NOT be used as a standalone KEM. */
/*     They are here for testing purposes, to ensure that everything works fine.      */
/*  The algorithms for the interface of the real Kyber KEM are in the second section  */
/**************************************************************************************/

/***********************/
/* INTERNAL ALGORITHMS */
/***********************/

void KEM_keygen_internal(uint8_t* ek, uint8_t* dk, const uint8_t* d, const uint8_t* z);

void KEM_encaps_internal(uint8_t* K, uint8_t* cipher, const uint8_t* ek, const uint8_t* msg);

// IL NE FAUT PAS AVOIR D'ORACLE D'ERREUR DE DECAPSULATION, SEULEMENT UN REJET IMPLICITE, IL FAUT DONC CHANGER EN void.
// POUR SAVOIR S'Il Y A EN EFFET EU UNE ERREUR, IL FAUT COMPARER QUE LA CLE PARTAGEE SOIT IDENTIQUE DANS LES DEUX CAS.
void KEM_decaps_internal(uint8_t* K, const uint8_t* dk, const uint8_t* cipher);

/***********************/
/* KYBER KEM INTERFACE */
/***********************/



#endif