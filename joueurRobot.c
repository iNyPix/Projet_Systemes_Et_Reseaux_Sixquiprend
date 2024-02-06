

#include "joueurRobot.h"
#include <time.h>
#include <stdlib.h>

int getNumCarte(Joueur* j)
{
    srand(time(NULL));
    int indice;
    indice = rand() % (j->mainJoueur.nbCartes);
    return indice;
}

int getRangee()
{
    srand(time(NULL));
    int indice;
    indice = rand() % 4 + 1;
    return indice;
}