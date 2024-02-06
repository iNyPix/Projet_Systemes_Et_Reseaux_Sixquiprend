/**
 * \file jeu.h
 * \brief Jeu.
 * \author Meryem BELASSEL & Thomas NICOLLE
 * \version 0.1
 * \date 22 décembre 2023
 *
 * Méthodes utilisées par le maitre du jeu.
 *
 */

#ifndef JEU_H
#define JEU_H

#include "Carte.h"

#define max 10

/**
 * \struct Table
 * \brief Représente la table du jeu.
 *
 * Table est une petite structure pour pouvoir placer les tables sur un plateau.
 */
typedef struct
{
    Carte plateau[4][6];/*!< Tableau de carte de dimension 4 rangées et 6 colonnes. */
}Table;


/**
 * \struct Proposition
 * \brief Structure pour stocker la carte choisie par un joueur.
 *
 * Proposition est une petite structure pour pouvoir enregistrer la proposition de debut de partie d'un joueur.
 */
typedef struct {
    Carte* carte_c;/*!< La carte proposée. */
    int ind; /*!< L'identifiant du joueur. */
} Proposition;

/**
 * \struct Joueur
 * \brief Structure d'un joueur.
 *
 * Joueur est une petite structure pour regroupe le paquet de carte et le nombre de pénalités d'un joueur.
 */
typedef struct
{
    Paquet mainJoueur; /*!< Main du joueur, peut contenir jusqu'à 10 cartes. */
    int penalite; /*!< Nombre de pénalités. */
}Joueur;

void initTable(Table *); // Pose des cartes vides
void initRangee(Table * ,Carte* , int );
Carte lastCarte(Table *, int);
int nbrCarteR(Table *, int );
bool ppCarteRangee(Table*, int, Joueur*);
bool poserCarteTable(Table *, int , int , Joueur*);
bool poserCarteInfTable(Table *, int , int , Joueur *);
void compterTeteDeBoeuf(Table *, int , Joueur * );

void distribuerCartes(Paquet*, Joueur [], int , int );
void afficherMainJoueur(Joueur*, char*, int);
void remplirPlateau(Paquet *, Table *);
void triCroissantP(Proposition [], int );
void affichagePlateau(Table *, char*, int);
bool verifScore(int a[max][2], Joueur[], int ,char*, int);

#endif //JEU_H
