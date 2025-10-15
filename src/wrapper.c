/**
 * @file wrapper.c
 * @brief Appelation de fonctions usuelles externes
 * @author Gabriel Abauzit
 *
 **/

 // Fonctions de hachage externes
extern void G(uint8_t out[64], const uint8_t* in, size_t inlen);
extern void PRF_eta(uint8_t* out, size_t outlen, const uint8_t s[32], uint8_t b);
extern void XOF_init(void);
extern void XOF_absorb(const uint8_t* in, size_t inlen);
extern void XOF_squeeze(uint8_t* out, size_t outlen);

/* A FAIREEEEEEEEEEE */

// Algorithme 13

void PRF(uint8_t* r, int size_r, uint8_t s[32], uint8_t b) {

}