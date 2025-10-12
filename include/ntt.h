#ifndef KYBER_NTT_H
#define KYBER_NTT_H

/*
 * @file ntt.h
 * @brief Implémentation de NTT et application à la multiplication rapide dans R_q
 * @author Gabriel Abauzit
 *
 */

#include "poly.h"

/*
 * Une NTT est de longueur 256 donc peut tout à fait être stockée dans le type poly_t.
 *
 * Ceci évite la duplication de la mémoire que causerait un type dédié, et permet de conserver la compatibilité avec toutes les fonctions programmées pour le type poly_t
 * dans le domaine NTT. En revanche, cela réduit légèrement la lisibilité du code, d'autres projets de référence ont également fait ce choix.
 * 
 */

/****************************/
/* REDUCTIONS DE MONTGOMERY */
/****************************/

/**
 * Tous les calculs de NTT (qui font donc intervenir des multiplications) ont lieu dans le domaine de Montgomery pour des raisons d'optimisation. Ceci implique plusieurs
 * considérations techniques, en particulier les tables de zeta dans ntt.c sont dans le domaine de Montgomery et les fonctions de réductions sont écrites dans ce fichier.
 * 
 * Cela signifie également qu'il faut ABSOLUMENT réduire tout polynôme sous sa forme de Montgomery avant de calculer sa NTT.
 * 
 */


/**
 * @brief Convert polynomial to Montgomery domain
 *
 * Multiplies all coefficients by R = 2^16 mod q
 * INPUT:  Polynomial in normal form [0, q-1]
 * OUTPUT: Polynomial in Montgomery form (in-place)
 *
 * @param r Polynomial to convert
 *
 * Time complexity: O(n), constant-time
 *
 * USAGE: Call this BEFORE first NTT operation!
 * Example:
 *   poly f;  // Normal form
 *   poly_tomont(&f);  // Now in Montgomery form
 *   ntt(&f);  // Can safely apply NTT
 */

void poly_to_montgomery(poly_t* f);

/**
 * @brief Convert polynomial from Montgomery domain to normal
 *
 * Multiplies all coefficients by R^-1 mod q
 * INPUT:  Polynomial in Montgomery form
 * OUTPUT: Polynomial in normal form [0, q-1] (in-place)
 *
 * @param r Polynomial to convert
 *
 * Time complexity: O(n), constant-time
 *
 * USAGE: Call this AFTER all NTT operations to get normal form!
 * Example:
 *   poly p;  // In Montgomery form after NTT/INTT
 *   poly_frommont(&p);  // Convert back to normal [0, q-1]
 */
void poly_from_montgomery(poly_t* f);

/***********************/
/* TRANSFORMATIONS NTT */
/***********************/

/**
 * @brief Transforme f en NTT(f)
 */

void NTT(poly_t* f);

/**
 * @brief Transforme f en NTT^{-1}(f)
 */

void NTT_inv(poly_t* f);

/**********************************/
/* OPERATIONS DANS LE DOMAINE NTT */
/**********************************/

/**
 * @brief Multiplie deux polynômes de degré 1 modulo x^2-m
 *
 * @param r_0 : coefficient de degré 0 du résultat
 * @param r_1 : coefficient de degré 1 du résultat
 * @param a_0 : coefficient de degré 0 du premier polynôme
 * @param a_1 : coefficient de degré 1 du premier polynôme
 * @param b_0 : coefficient de degré 0 du second polynôme
 * @param b_1 : coefficient de degré 1 du second polynôme
 * @param m : le produit est calculé modulo x^2-m (doit provenir du tableau zetas_basemul)
 *
 */

void BaseCaseMultiply(int16_t* r0, int16_t* r1, const int16_t* a0, const int16_t* a1, const int16_t* b0, const int16_t* b1, const int16_t* m);

/**
 * @brief Multiplie deux NTT entre elles
 *
 * @param r : résultat
 * @param a : première NTT
 * @param b : seconde NTT
 *
 */

void MultiplyNTT(poly_t *r, const poly_t* a, const poly_t* b);

/****************************/
/* MULTIPLICATIONS AVEC NTT */
/****************************/

/**
 * @brief Multiplie deux polynômes entre eux via la NTT
 *
 * @param r : résultat
 * @param a : premier polynôme
 * @param b : second polynôme
 *
 */

void NTT_mult(poly_t* r, poly_t* a, poly_t* b);

//RAJOUTER LES PRODUITS SCALAIRE ET LE PRODUIT MATRICE/VECTEUR

#endif