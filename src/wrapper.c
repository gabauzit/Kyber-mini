/**
 * @file wrapper.c
 * @brief FIPS 203 Section 4.1
 * @author Gabriel Abauzit
 */

#include "wrapper.h"

/**
 * @brief Pseudo Random Function
 * @details Assign the value SHAKE256(s||b, 8*64*eta) to the variable pointed to by r
 * 
 * @param r[out]
 * @param eta[in]
 * @param s[in]
 * @param b[in]
 */
void PRF(uint8_t* r, const uint8_t eta, const uint8_t* s, const uint8_t b) {

}

/**
 * @brief Hash function
 * @details Assigns the value SHA3-256(s) to the variable pointed to by r
 * 
 * @param r[out]
 * @param s[in]
 */
void H(uint8_t* r, const uint8_t* s) {

}

/**
 * @brief Hash function
 * @details Assigns the value SHAKE256(s, 8*32) to the variable pointed to by r
 * 
 * @param r[out]
 * @param s[in]
 */
void J(uint8_t* r, const uint8_t* s) {

}

/**
 * @brief Hash function
 * @details Assigns the value SHA3-512(c) to the variable pointed to by r
 * 
 * @param r[out]
 * @param s[in]
 */
void G(uint8_t* r, const uint8_t* c) {
    
}

/********************************************************************/
/* STEPS FOR INCREMENTAL XOF, USED ONLY IN sampling.c WITH SHAKE128 */
/********************************************************************/

void XOF_init() {
    return;
}

void XOF_absorb() {
    return;
}

void XOF_squeeze() {
    return;
}