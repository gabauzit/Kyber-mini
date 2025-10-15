#ifndef NTT_H
#define NTT_H

/*
 * @file ntt.h
 * @brief Impl�mentation de NTT et application � la multiplication rapide dans R_q
 * @author Gabriel Abauzit
 *
 */

#include "poly.h"

 /*
  * Une NTT est de longueur 256 donc peut tout � fait �tre stock�e dans le type poly_t.
  *
  * Ceci �vite la duplication de la m�moire que causerait un type d�di�, et permet de conserver la compatibilit� avec toutes les fonctions programm�es pour le type poly_t
  * dans le domaine NTT. En revanche, cela r�duit l�g�rement la lisibilit� du code, d'autres projets de r�f�rence ont �galement fait ce choix.
  *
  */

  /****************************/
  /* REDUCTIONS DE MONTGOMERY */
  /****************************/

  /**
   * Tous les calculs de NTT (qui font donc intervenir des multiplications) ont lieu dans le domaine de Montgomery pour des raisons d'optimisation. Ceci implique plusieurs
   * consid�rations techniques, en particulier les tables de zeta dans ntt.c sont dans le domaine de Montgomery et les fonctions de r�ductions sont �crites dans ce fichier.
   *
   * Cela signifie �galement qu'il faut ABSOLUMENT r�duire tout polyn�me sous sa forme de Montgomery avant de calculer sa NTT.
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
 * @brief Multiplie deux polyn�mes de degr� 1 modulo x^2-m
 *
 * @param r_0 : coefficient de degr� 0 du r�sultat
 * @param r_1 : coefficient de degr� 1 du r�sultat
 * @param a_0 : coefficient de degr� 0 du premier polyn�me
 * @param a_1 : coefficient de degr� 1 du premier polyn�me
 * @param b_0 : coefficient de degr� 0 du second polyn�me
 * @param b_1 : coefficient de degr� 1 du second polyn�me
 * @param m : le produit est calcul� modulo x^2-m (doit provenir du tableau zetas_basemul)
 *
 */

void BaseCaseMultiply(int16_t* r0, int16_t* r1, const int16_t* a0, const int16_t* a1, const int16_t* b0, const int16_t* b1, const int16_t* m);

/**
 * @brief Multiplie deux NTT entre elles
 *
 * @param r : r�sultat
 * @param a : premi�re NTT
 * @param b : seconde NTT
 *
 */

void MultiplyNTT(poly_t* r, const poly_t* a, const poly_t* b);

#endif