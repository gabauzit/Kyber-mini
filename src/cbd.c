//N'EST PAS DU TOUT VIABLE, REMPLACER A TERME PAR UN PRNG
int random_bit() {
    return rand() % 1;
}


/*
 * @brief Renvoie un élément pseudo-aléatoire de R_q dont les coefficients suivent une loi CBD
 *
 * @param r pointeur vers le polynôme de sortie
 * @param B tableau de 2*eta*KYBER_N bits pseudo-aléatoires
 * @param size_B taille du tableau B
 *
 * @note eta est implicitement défini par l'égalité size_B = 2*eta*KYBER_N
 */


void SamplePolyCBD(poly_t* r, int* B, int size_B) {
    unsigned eta = size_B / (2 * KYBER_N);
    int i, j;

    for (i = 0; 2 * i < KYBER_N; i++) {
        for (j = 0; j < eta; j++) {
            r->coeffs[i] += B[2 * i * eta + j] % KYBER_Q; //REMPLACER PAR UNE VRAIE MULTIPLICATION MODULO KYBER_Q AVEC REDUCTION DE MONTGOMERY
            r->coeffs[i] -= B[2 * i * eta + eta + j] % KYBER_Q;
        }
    }
}

/*
 * @brief Renvoie un vecteur pseudo-aléatoire à coefficients dans R_q
 *
 *
 *
 * @note eta est implicitement défini par l'égalité size_B = 2*eta*k*KYBER_N
 */
void SamplePolyVectorCBD(poly_t** r, unsigned k, int** B, int size_B) {
    unsigned eta = size_B / (2 * k * KYBER_N);
    unsigned size_B_bis = size_B / k;
    int i;

    for (i = 0; i < k; i++) {
        SamplePolyCBD(r[i], B[i], size_B_bis);
    }
}