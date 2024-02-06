
#include "jeu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Initialiser la rangée */
void initRangee(Table *t, Carte *ct, int l) {
    t->plateau[l][0] = *ct;
    for (int i = 1; i < 6; i++) {
        t->plateau[l][i] = carteVide();
    }
}

Carte lastCarte(Table *t, int rangee) {
    int colonne = 0;
    while (!comparerCarte(t->plateau[rangee][colonne], carteVide())) {
        colonne++;
    }
    return t->plateau[rangee][colonne - 1];
}

int nbrCarteR(Table *t, int rangee) {
    int colonne = 0;
    while (!comparerCarte(t->plateau[rangee][colonne], carteVide())) {
        colonne++;
    }
    return colonne;
}

void triCroissantP(Proposition t[], int nbrJoueurs) {
    for (int j = 0; j < nbrJoueurs - 1; ++j) {
        for (int i = 0; i < nbrJoueurs - 1; ++i) {
            if (t[i].carte_c->num > t[i + 1].carte_c->num) {
                Proposition tmp;
                tmp = t[i];
                t[i] = t[i + 1];
                t[i + 1] = tmp;
            }
        }
    }
}

void compterTeteDeBoeuf(Table *t, int ligne, Joueur *j) { // Prend les cartes d'une rangée
    for (int i = 0; i < 6; ++i) {
        j->penalite += t->plateau[ligne][i].valBoeuf;
    }
}

bool poserCarteTable(Table *t, int indiceCarte, int rangee, Joueur *j) {
    bool res = true;
    if ((rangee > 0) && (rangee < 5)) {
        int difference;
        int min = 105;
        int indiceRangee = 0;
        Carte c1 = j->mainJoueur.tab[indiceCarte];
        for (int i = 0; i < 4; ++i) {
            Carte c2 = lastCarte(t, i);
            if (c2.num < c1.num) {
                difference = c1.num - c2.num;
                if (difference < min) {
                    min = difference;
                    indiceRangee = i;
                }
            }
        }
        // Verification
        if ((rangee - 1) != indiceRangee) {
            res = false;
            printf("Mauvaise position, réessayez.\n");
        } else {

            int nbrCartes = nbrCarteR(t, indiceRangee); // Retourne le nombre de cartes qui sont sur la rangée i
            Carte carte_retiree;

            if (nbrCartes < 5) // Si la rangée n'est pas complète
            {
                carte_retiree = tireCarte(&j->mainJoueur, indiceCarte);
                t->plateau[indiceRangee][nbrCartes] = carte_retiree; //poser la carte à la position nbrCartes if () // Si la rangée n'est pas complète
                res = true;
            } else // Si la rangée est complète
            {
                carte_retiree = tireCarte(&j->mainJoueur, indiceCarte);
                compterTeteDeBoeuf(t, indiceRangee, j);
                // on réinitialise la rangée et on place à la première colonne de la rangée la carte c1
                initRangee(t, &carte_retiree, indiceRangee);
                res = true;
            }
        }
    } else {
        res = false;
        printf("Erreur, position hors du plateau\n");
    }
    return res;
}

void distribuerCartes(Paquet *p, Joueur joueurs[], int nbJoueurs, int cartesParJoueur) {
    for (int i = 0; i < nbJoueurs; i++) {
        for (int j = 0; j < cartesParJoueur; j++) {
            if (p->nbCartes > 0) {
                joueurs[i].mainJoueur.tab[joueurs[i].mainJoueur.nbCartes] = tireCarte(p, -1);
                joueurs[i].mainJoueur.nbCartes++;
            }
        }
    }
}

// Afficher, le paquet de cartes d'un joueur
void afficherMainJoueur(Joueur *j, char *buffer, int taillebuf) {
    char *aff = (char *) malloc(taillebuf * sizeof(char));
    bzero(aff, taillebuf);
    strcat(aff, "╭-------- PAQUET DES CARTES --------╮\n|");
    triCroissant(&(j->mainJoueur));
    affichePaquet(&(j->mainJoueur), aff);
    strcat(aff, "╰----------------------------------╯\n");
    strcpy(buffer, aff);
    free(aff);
}

/* Initialisation du plateau du jeu*/
void initTable(Table *t) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 6; j++) {
            t->plateau[i][j] = carteVide();
        }
    }
}

// Pour initier le jeu, on place les 4 cartes sur les 4 rangées
void remplirPlateau(Paquet *p, Table *t) {
    for (int i = 0; i < 4; i++) {
        t->plateau[i][0] = tireCarte(p, -1);
    }
}

void affichagePlateau(Table *t, char *buffer, int taillebuf) {
    char *buf = (char *) malloc(taillebuf * sizeof(char));
    snprintf(buf, taillebuf, "╭--------|PLATEAU|--------╮\n");
    for (int i = 0; i < 4; i++) {
        strcat(buf,"|");
        for (int j = 0; j < 5; j++) {
            char *temp = (char *) malloc(taillebuf * sizeof(char));
            bzero(temp,taillebuf);
            afficheCarte(&(t->plateau[i][j]), temp);
            strcat(buf, temp);
            free(temp);
        }
        strcat(buf, "|\n");
    }
    strcat(buf, "╰------------------------╯\n");
    strcat(buffer, buf);
    free(buf);
}

bool ppCarteRangee(Table *t, int val, Joueur *j) {
    Carte cComp = j->mainJoueur.tab[val];
    int i = 0;
    for (int k = 0; k < 4; ++k) {
        Carte last = lastCarte(t, k);
        if (last.num > cComp.num) i++;
    }
    return (i == 4);
}

bool poserCarteInfTable(Table *t, int indiceCarte, int rangee, Joueur *j) {
    Carte carte_retiree;
    carte_retiree = tireCarte(&j->mainJoueur, indiceCarte);
    compterTeteDeBoeuf(t, rangee - 1, j);
    // on réinitialise la rangée et on place à la première colonne de la rangée la carte c1
    initRangee(t, &carte_retiree, rangee - 1);
//    printf("La carte qui remplace est de num : %d", t->plateau[rangee - 1][0].num);
    return true;
}

/* Méthode qui vérifie les scores, les affiche, affiche un classement si: score = 66 */
bool verifScore(int classement[max][2], Joueur lesjoueurs[], int nbrJoueurs, char *buf, int taillebuf) {
    bool perdu = false;
    for (int i = 0; i < nbrJoueurs; ++i) { // Vérifie si au moins un joueur à un score de 66
        classement[i][0] = i;
        classement[i][1] = lesjoueurs[i].penalite;
        if (lesjoueurs[i].penalite >= 66)
            perdu = true;
    }
    if (perdu) // Si un joueur a atteint 66 têtes de bœuf -> trie du classement
    {
        for (int i = 0; i < nbrJoueurs - 1; ++i) {
            for (int j = 0; j < nbrJoueurs - 1; ++j) {
                if (classement[i][1] > classement[i + 1][1]) {
                    int idTemp = classement[i][0];
                    int penaliteTemp = classement[i][1];
                    classement[i][0] = classement[i + 1][0];
                    classement[i][1] = classement[i + 1][1];
                    classement[i + 1][0] = idTemp;
                    classement[i + 1][1] = penaliteTemp;
                }
            }
        }
    }
    char *score = (char *) malloc(taillebuf * sizeof(char));
    bzero(score, taillebuf);
    for (int i = 0; i < nbrJoueurs; ++i) {
        char* temp = (char*) malloc(taillebuf * sizeof(char));
        bzero(temp, taillebuf);
        snprintf(temp,taillebuf ,"Le score du joueur %d : %d\n", classement[i][0], classement[i][1]);
        strcat(score,temp);
        free(temp);
    }
    strcpy(buf, score);
    free(score);
    return perdu;
}