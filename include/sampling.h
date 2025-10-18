/**
 * @file sampling.h
 * @brief 
 * @author Gabriel Abauzit
 */

#ifndef SAMPLING_H
#define SAMPLING_H

#include "encode.h"
#include "wrapper.h"

void sample_ntt(int16_t* r, const uint8_t* seed);

void sample_poly_cbd(int16_t* r, const uint8_t* seed, const uint8_t eta);

#endif