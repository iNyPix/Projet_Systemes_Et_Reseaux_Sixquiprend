/**
 * \file joueurRobot.h
 * \brief Joueur Robot.
 * \author Meryem BELASSEL & Thomas NICOLLE
 * \version 0.1
 * \date 22 décembre 2023
 *
 * Programme robot pour jouer au jeu 6 qui prend.
 *
 */

#ifndef JOUEURROBOT_H
#define JOUEURROBOT_H

#include "jeu.h"

int getNumCarte(Joueur*); // Retourne aléatoirement l'indice d'une carte entre 0 et nbCartes du paquet du joueur
int getRangee();
#endif //JOUEURROBOT_H
