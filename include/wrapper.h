/**
 * @file wrapper.h
 * @brief FIPS 203 Section 4.1
 * @author Gabriel Abauzit
 */

#ifndef WRAPPER_H
#define WRAPPER_H

#include <stdint.h>

typedef struct {
    shake128ctx ctx; // LE TYPE UTILISÉ PAR PQClean, il faut include "fips202.h"
} XOF_ctx;


void PRF(uint8_t* r, const uint8_t eta, const uint8_t* s, const uint8_t b);

void H(uint8_t* r, const uint8_t* s);

void J(uint8_t* r, const uint8_t* s);

void G(uint8_t* r, const uint8_t* c);

/********************************************************************/
/* STEPS FOR INCREMENTAL XOF, USED ONLY IN sampling.c WITH SHAKE128 */
/********************************************************************/

// IL FAUT PLUS D'ARGUMENTS DANS CHACUNE DES FONCTIONS

void XOF_init(XOF_ctx* ctx);

void XOF_absorb(XOF_ctx* ctx, const uint8_t* seed);

void XOF_squeeze(uint8_t* output, XOF_ctx* ctx, const unsigned len);

#endif