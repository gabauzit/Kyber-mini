# Kyber-mini
Implémentation d'un mini-Kyber, mesure de performances et de sécurité

**NE PAS UTILISER EN SITUATION REELLE**

## Introduction

Présentation du projet
Respect des codes de sécurité (gestion de mémoire, algo en temps constant)

## Explication de l'algorithme Kyber-mini

Pourquoi travailler dans l'anneau $R_q$ ? L'anneau $R_q$ est une des façons de représenter un $n$-uplet de d'éléments de Z_q, mais on pourrait pour ça remplacer x^n+1 par n'importe quel polynôme de degré n à coefficient dans Z_q. Le choix de x^n+1 est motivé par l'observation suivante : prenons deux polynômes a(x),b(x) dans R_q, 
$$
(ab)_k = \sum_{i+j\equiv k\pmod{n}}\varepsilon_{i,j}a_ib_j
$$

## Mesure de performances

Comparaison produit de polynômes naïf VS NTT pour différents n
Mesure de performance selon les paramètres

## Mesure de sécurité

Mesure le temps de calcul d'une attaque naïve et rafinnée sur des petits paramètres.
Estimer la durée correspondante pour des paramètres réels.