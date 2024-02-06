/**
 * \file gestionnaireDeJeu.h
 * \brief Gestionnaire de jeu.
 * \author Meryem BELASSEL & Thomas NICOLLE
 * \version 0.1
 * \date 22 décembre 2023
 *
 * Programme serveur pour jouer au jeu 6 qui prend.
 *
 */

#ifndef PROJET_GESTIONNAIREDEJEU_H
#define PROJET_GESTIONNAIREDEJEU_H

#include <semaphore.h>

/**
 * \struct connexionJoueurH
 * \brief stocker les données d'un client.
 *
 * connexionJoueurH est une petite structure pour les connexion des joueurs humains.
 * La structure enregistre le descripteur du socket client ainsi que l'identifiant du joueur
 */
typedef struct {
    int socket;/*!< Descripteur du socket joueur. */
    int id;/*!< Identifiant du joueur. */
} connexionJoueurH;

/* Signatures méthodes */

/**
 * \fn clean_saisieClavier(void)
 * \brief Nettoyer le tampon d'entrée (stdin), qui stocke temporairement les saisies clavier.
 *
 */
void clean_saisieClavier(void);

void semaphore_wait(sem_t*, int);
void attenteJoueurs(int *);
void* GestionConnexionHumains(void*);
void* GestionConnexionRobots(void*);

void sendToPlayer(connexionJoueurH *, char *);
void recevFromPlayer(connexionJoueurH *, char *);

#endif //PROJET_GESTIONNAIREDEJEU_H
