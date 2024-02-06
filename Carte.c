/**
 * \file Carte.c
 * \brief Carte.
 * \author Meryem BELASSEL & Thomas NICOLLE
 * \version 0.1
 * \date 22 décembre 2023
 *
 * Définition des méthodes liées à une carte ou un paquet.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Carte.h"

#define BUFFER_SIZE 500

Carte carteVide()
{
    Carte vide;
    vide.valBoeuf = 0;
    vide.num = 0;

    return vide;
}

bool comparerCarte(Carte  c1,Carte  c2)
{
    if (c1.valBoeuf == c2.valBoeuf && c1.num == c2.num)
        return true;
    else
        return false;
}

void fillPaquet(Paquet *p)
{
    for (int i = 1; i <= NBMAX; i++)
    {
        p->tab[i - 1].num = i;
        p->tab[i - 1].valBoeuf = 1; // Par défaut

        if (i % 10 == 0) // Pour les cartes de valeur numérique multiple de 10
        {
            p->tab[i - 1].valBoeuf = 3;
        }
        else if (i % 10 == 5) // pour les cartes de valeur numérique qui se termine aevc un 0
        {
            p->tab[i - 1].valBoeuf = 2;
        }

        if (i % 11 == 0) // pour les cartes de valeur numérique multiple de 11
        {
            p->tab[i - 1].valBoeuf = 5;
        }

        if (i % 11 == 0 && i % 10 == 5) // pour les cartes de valeur numérique multiple de 11 et qui se termine avec 5
        {
            p->tab[i - 1].valBoeuf = 7;
        }
    }
    p->nbCartes = NBMAX;
}

void afficheCarte(Carte *c, char* buffer)
{
    char* b = (char *) malloc(BUFFER_SIZE*sizeof(char));
    bzero(b, BUFFER_SIZE);
    snprintf(b,BUFFER_SIZE ,"[n°%d , %d boeuf] ", c->num, c->valBoeuf);
    strcpy(buffer,b);
    free(b);
}

void affichePaquet(Paquet *p, char* buffer)
{
    char* b = (char *)malloc(BUFFER_SIZE * sizeof(char));
    bzero(b, BUFFER_SIZE);
    for (int i = 0; i < p->nbCartes; i++)
    {
        char* tmp = (char*) malloc(BUFFER_SIZE*sizeof(char));
        bzero(tmp, BUFFER_SIZE);
        afficheCarte(&p->tab[i], tmp);
        strcat(b,tmp);
        free(tmp);
    }
    strcat(b, "\n");
    strcat(buffer, b);
    free(b);
}

void melangePaquet(Paquet *p)
{
    for (int i = 0; i < p->nbCartes; i++)
    {
        int nbal = (rand() % (p->nbCartes));
        Carte tmp = p->tab[i];
        p->tab[i] = p->tab[nbal];
        p->tab[nbal] = tmp;
    }
}


Carte tireCarte(Paquet *p, int indice)
{
    if(p->nbCartes > 0)
    {
        if(indice == -1) // Prend carte aléatoirement
        {
            p->nbCartes--;
            return p->tab[p->nbCartes];
        }
        else if(!comparerCarte(p->tab[indice] , carteVide() )  ||  (indice >= 0 && indice < p->nbCartes) ) // Vérifie si la carte existe et si non vide
        {
            Carte carteChoisie = p->tab[indice];
            // Déplacer les éléments suivants dans le tableau pour remplir l'emplacement de la carte retirée
            for (int i = indice; i < p->nbCartes - 1; ++i) {
                p->tab[i] = p->tab[i + 1];
            }
            p->nbCartes--;
            return carteChoisie;
        }
        else{
            if (comparerCarte(p->tab[indice] , carteVide() ) )
                printf("ERREUR: Carte vide\n");
            else
                printf("Indice invalide pour tirer une carte\n");
        }
    }
    else
    {
        printf("Impossible de tirer une Carte dans un Paquet vide\n");
    }
}

void triCroissant(Paquet *p) {
    Carte tmp;
    for (int j = 0; j < (p->nbCartes - 1); j++) {
        for (int i = 0; i < (p->nbCartes - 1); i++) {
            if (p->tab[i].num > p->tab[i + 1].num) {
                tmp = p->tab[i];
                p->tab[i] = p->tab[i + 1];
                p->tab[i + 1] = tmp;
            }
        }
    }
}