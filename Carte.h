//
// Created by belassel/nicolle on 30/10/2023.
//
#ifndef CARTE_H
#define CARTE_H

#include <stdbool.h>

#define NBMAX 104

// Attributes

/* Structure d'une carte */
typedef struct
{
    int valBoeuf;
    int num; // de 1 à 104
}Carte;

/* Structure d'un paquet de cartes*/
typedef struct
{
    int nbCartes;
    Carte tab[NBMAX];
}Paquet;

/* Signatures méthodes */
Carte carteVide();
bool comparerCarte(Carte ,Carte );
void afficheCarte(Carte*, char*);
void fillPaquet(Paquet*);
void affichePaquet(Paquet*, char*);
void melangePaquet(Paquet* );
Carte tireCarte(Paquet*, int indice);
void triCroissant(Paquet *p);


#endif //PROJET_CARTE_H
