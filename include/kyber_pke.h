/**
 * @file kyber_pke.h
 * @brief Public Key Encryption part of Kyber (should NOT be used independently on its own)
 * @author Gabriel Abauzit
 */

#ifndef KYBER_PKE_H
#define KYBER_PKE_H

#include <stdint.h>
#include <string.h>
#include "consts.h"
#include "encode.h"
#include "poly.h"
#include "polyvec.h"
#include "reduce.h"
#include "sampling.h"
#include "wrapper.h" //FAIRE TOUTES CES INCLUSIONS DANS LES AUTRES FICHIERS, ON IMPORTE TOUT CE QU'ON UTILISE !!!!!!!!!!!!!!!!!!!!!!!!!!

void PKE_keygen(uint8_t* ek, uint8_t* dk, const uint8_t* d);

void PKE_encrypt(uint8_t* cipher, const uint8_t* ek, const uint8_t* msg, const uint8_t* random);

void PKE_decrypt(uint8_t* msg, const uint8_t* dk, const uint8_t* cipher);

#endif