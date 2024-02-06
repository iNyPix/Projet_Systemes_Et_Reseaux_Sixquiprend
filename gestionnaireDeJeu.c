
/* Bibliothèques C */
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
/* Bibliothèques du jeu*/
#include "gestionnaireDeJeu.h"
#include "jeu.h"
#include "joueurRobot.h"
#include "gestionnaireDeStats.h"

/* Constantes */
#define PORT 8087 // Port d'écoute du mj
#define BUFFER_SIZE 500 // Taille du buffer de lecture
#define MAX_JOUEURS 10 // Nombre de joueurs
#define MAX_MANCHE 1 // Nombre de manche désiré


/* Variables de jeux */
pthread_t threads[MAX_JOUEURS]; // Threads CnxJ et CnxR
connexionJoueurH cnxJoueurH[MAX_JOUEURS]; // Connexions joueurs

Joueur mains[MAX_JOUEURS]; // Mains des joueurs et des robots&
Proposition tabCartes_c[MAX_JOUEURS]; // Propositions de debut de tour des joueurs

int score[MAX_JOUEURS][2]; // Scoreboard des joueurs
char stats[500]; // buffer pour remplir le fichier des stats

/* Semaphores */
sem_t sem_thread[MAX_JOUEURS]; // Semaphore pour les joueurs
sem_t sem_GJ; // Semaphore du maitre du jeu
Table plateauDuJeu;

/* Variables globales */
int JoueurCount = 0; // nbr actuel de joueurs
int RobotCount = 0; // nbr actuel de robots
int Tour = 1; // Tour actuel
bool maxTetes = false; // nbr max de tetes atteinte ?


/* nettoyer le tampon d'entrée (stdin), qui stocke temporairement les saisies clavier */
void clean_saisieClavier(void) {
    char saisie;
    while ((saisie = getchar()) != '\n' && saisie != EOF) {}
}

/* Fonction pour wait sur un semaphore nbr fois */
void semaphore_wait(sem_t *s, int nbr) {
    for (int i = 0; i < nbr; ++i) {
        sem_wait(s);
    }
}

/* Fonction de réception d'un buffer d'un joueur */
void recevFromPlayer(connexionJoueurH *j, char *bfr) {
    bzero(bfr, BUFFER_SIZE);
    char *buffer = (char *) malloc(sizeof(char));
    bzero(buffer, BUFFER_SIZE);
    recv(j->socket, buffer, BUFFER_SIZE, 0);
    strcpy(bfr, buffer);
    free(buffer);
}

/* Fonction d'envoi d'un buffer à un joueur */
void sendToPlayer(connexionJoueurH *j, char *bfr) {
    char *buffer = (char *) malloc(sizeof(char));
    bzero(buffer, BUFFER_SIZE);
    snprintf(buffer, BUFFER_SIZE, "%s\n", bfr);
    send(j->socket, buffer, BUFFER_SIZE, 0);
    free(buffer);
}

/* Fonction exécutée par chaque thread joueur */
void *GestionConnexionHumains(void *joueur_data) {
    connexionJoueurH *joueur = (connexionJoueurH *) joueur_data;
    int index = joueur->id;
    int joueurSocket = joueur->socket;
    char buffer[BUFFER_SIZE]; // buffer de lecture
    char intro[1000];
    printf("[+] Joueur %d s'est connecté au Jeu !\n", index + 1);
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "[+] Joueur %d s'est connecté au Jeu !\n", index + 1);
    logStats(stats);
    sprintf(intro, "╒=========================================================================╕\n"
                   "|               .::Bienvenue dans le jeu 6 Qui Prend! ::.                 |\n"
                   "╞=========================================================================╡\n"
                   "|Principe du jeu: Les cartes de « 6 qui prend » ont 2 valeurs : une valeur|\n"
                   "|numérique (de 1 à 104) qui indique leur future position                  |\n"
                   "|dans le jeu, et une valeur de 1 à 7 « têtes de boeufs » ,                |\n"
                   "|qui correspond à des points de pénalité. Le but est                      |\n"
                   "|de récolter le moins possible de têtes de boeufs. Le                     |\n"
                   "|gagnant est celui qui en comptabilise le moins à la fin du jeu.          |\n"
                   "╘=========================================================================╛\n"
                   "En attente des joueurs...\n");
    send(cnxJoueurH[index].socket, intro, sizeof(intro), 0); // envoie msg debut Tour
    sem_wait(&sem_thread[index]); // Blocage pour attendre les autres joueurs
    /*           SEM MJ  1            */
    sem_post(&sem_GJ); // Débloque le GJ
    /*           SEM MJ  1            */
    // MJ prépare le plateau
    sem_wait(&sem_thread[index]);// Blocage avant le début de la Tour
    bzero(buffer, BUFFER_SIZE);
    affichagePlateau(&plateauDuJeu, buffer, BUFFER_SIZE);
    printf("[MJ] Envoie plateau au joueur %d\n", index + 1);
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "[MJ] Envoie plateau au joueur %d\n", index + 1);
    logStats(stats);
    send(cnxJoueurH[index].socket, buffer, BUFFER_SIZE, 0); // envoie plateau
    bzero(buffer, BUFFER_SIZE);
    do {
        bzero(buffer, BUFFER_SIZE);
        sprintf(buffer, " .::DEBUT DU TOUR %d::. \n", Tour);
        logStats(" .::DEBUT DU TOUR %d::. \n");
        send(cnxJoueurH[index].socket, buffer, BUFFER_SIZE, 0); // envoie msg debut Tour
        bzero(buffer, BUFFER_SIZE);
        afficherMainJoueur(&mains[index], buffer, BUFFER_SIZE);
        printf("[MJ] Envoie main au joueur %d\n", index + 1);
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "[MJ] Envoie main au joueur %d\n", index + 1);
        logStats(stats);
        send(cnxJoueurH[index].socket, buffer, BUFFER_SIZE, 0); // envoie main joueur
        snprintf(buffer, BUFFER_SIZE, " Choisissez une carte !\n");
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), " Choisissez une carte !\n");
        logStats(stats);
        send(cnxJoueurH[index].socket, buffer, BUFFER_SIZE, 0); // demander au joueur de choisir la carte
        bzero(buffer, BUFFER_SIZE);
        printf("[MJ] Attente d'une réponse du joueur %d \n", index + 1);
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "[MJ] Attente d'une réponse du joueur %d \n", index + 1);
        logStats(stats);
        recv(cnxJoueurH[index].socket, buffer, BUFFER_SIZE, 0); // Recevoir carte choisie par le joueur
        int valNum = atoi(buffer) - 1;
        Proposition prop;
        prop.carte_c = &mains[index].mainJoueur.tab[valNum]; // récupère la carte choisie dans la main du joueur
        prop.ind = index;
        tabCartes_c[index] = prop; // Place la proposition dans le tableau
        printf("[MJ] Le joueur %d a proposé la carte [valNum=%d, %d boeuf] \n", index + 1,
               mains[index].mainJoueur.tab[valNum].num, mains[index].mainJoueur.tab[valNum].valBoeuf);
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "[MJ] Le joueur %d a proposé la carte [valNum=%d, %d boeuf] \n", index + 1,
                 mains[index].mainJoueur.tab[valNum].num, mains[index].mainJoueur.tab[valNum].valBoeuf);
        logStats(stats);
        /*           SEM MJ  2            */
        sem_post(&sem_GJ); // Déblocage du gj pour veri
        /*           SEM MJ  2            */
        sem_wait(&sem_thread[index]);// Blocage du joueur avant verif du gj
        bool placement_possible = true;
        bool carte_petite = ppCarteRangee(&plateauDuJeu, valNum, &mains[index]);
        do {
            bzero(buffer, BUFFER_SIZE);
            affichagePlateau(&plateauDuJeu, buffer, BUFFER_SIZE);
            sendToPlayer(joueur, buffer);
            printf("Le joueur %d va recevoir le message pose carte\n", index + 1);
            bzero(stats, sizeof(stats));
            snprintf(stats, sizeof(stats), "Le joueur %d va recevoir le message pose carte\n", index + 1);
            logStats(stats);
            if (!carte_petite) {
                sendToPlayer(joueur, " Pour poser la carte, veuillez entrer la rangée :\n");
                recevFromPlayer(joueur, buffer);
            } else {
                sendToPlayer(joueur, " Votre carte est la plus petite, veuillez choisir une rangée à récupérer :\n");
                recevFromPlayer(joueur, buffer);
            }
            int numRangee = atoi(buffer);
            printf("[MJ] Le joueur %d place sa carte à la rangée %d\n", index, numRangee);
            bzero(stats, sizeof(stats));
            snprintf(stats, sizeof(stats), "[MJ] Le joueur %d place sa carte à la rangée %d\n", index, numRangee);
            logStats(stats);
            if (carte_petite)
                placement_possible = poserCarteInfTable(&plateauDuJeu, valNum, numRangee, &mains[index]);
            else
                placement_possible = poserCarteTable(&plateauDuJeu, valNum, numRangee, &mains[index]);
            if (placement_possible)
                sendToPlayer(joueur, "1");
            else
                sendToPlayer(joueur, "0");

        } while (!placement_possible);
        bzero(buffer, BUFFER_SIZE);
        affichagePlateau(&plateauDuJeu, buffer, BUFFER_SIZE);
        printf("MISE A JOUR DU PLATEAU :\n %s\n", buffer);
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "MISE A JOUR DU PLATEAU :\n %s\n", buffer);
        logStats(stats);
        bzero(buffer, BUFFER_SIZE);
        maxTetes = verifScore(score, mains, JoueurCount+RobotCount, buffer, BUFFER_SIZE);

        printf("MaxTetes = %d tour= %d\n",maxTetes,Tour);
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "MaxTetes = %d tour= %d\n",maxTetes,Tour);
        logStats(stats);
        if (Tour == (10 * MAX_MANCHE) || maxTetes) // Fin de partie
        {
            strcat(buffer, "Manche terminée.");
            sendToPlayer(joueur, buffer);
        } else { // Si partie continue
            sendToPlayer(joueur, buffer);
        }

        sem_post(&sem_GJ); // Libère le MJ pour faire jouer les autres joueurs
        sem_wait(&sem_thread[index]); // Le thread attend après avoir posé la carte
        /* Fin d'un Tour */
    }while (Tour <= (10 * MAX_MANCHE) || maxTetes); // 10 cartes non jouées
    // FIN DE PARTIE
    printf(" JEU TERMINE !\n");
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "JEU TERMINE !\n");
    logStats(stats);
    // Si le joueur se déconnecte, fermez le socket et terminez le thread
    close(joueurSocket);
    joueurSocket = -1;
    pthread_exit(NULL);
}

/* Fonction exécutée par chaque thread joueur */
void *GestionConnexionRobots(void *robot_id) {
    int index = *(int *) robot_id;
    char buf[BUFFER_SIZE];
    printf("[+] Robot %d s'est connecté au Jeu !\n", index);
    sem_wait(&sem_thread[index]); // Blocage avant le début de la Tour
    /*           SEM MJ  1            */
    sem_post(&sem_GJ); // Débloque le GJ
    /*           SEM MJ  1            */
    // MJ prépare le plateau
    sem_wait(&sem_thread[index]);// Blocage avant le début de la Tour
    do {
        // demander au robot de choisir la carte
        int val = getNumCarte(&mains[index]);
        // Recevoir carte choisie par le joueur
        Proposition prop;
        prop.carte_c = &mains[index].mainJoueur.tab[val]; // récupère la carte choisie dans la main du joueur
        prop.ind = index;
        tabCartes_c[index] = prop; // Place la proposition dans le tableau
        printf("[MJ] Le robot %d a proposé la carte [val=%d, %d boeuf] \n", index,
               mains[index].mainJoueur.tab[val].num, mains[index].mainJoueur.tab[val].valBoeuf);
        /*           SEM MJ  2            */
        sem_post(&sem_GJ); // Déblocage du gj pour verif
        /*           SEM MJ  2            */
        sem_wait(&sem_thread[index]);// Blocage du robot avant verif du gj
        bool p_possible = true;
        bool carte_petite = ppCarteRangee(&plateauDuJeu, val, &mains[index]);
        do {
            int numRangee = getRangee();
            printf("[MJ] Le robot %d place sa carte à la rangée %d\n", index, numRangee);
            if (carte_petite)
            {
                p_possible = poserCarteInfTable(&plateauDuJeu, val, numRangee, &mains[index]);
            }
            else {
                p_possible = poserCarteTable(&plateauDuJeu, val, numRangee, &mains[index]);
            }
        } while (!p_possible);

        // maj du plateau
        bzero(buf, BUFFER_SIZE);
        maxTetes = verifScore(score, mains, JoueurCount+RobotCount, (char *) &buf, BUFFER_SIZE);
        if (Tour == (10 * MAX_MANCHE) || maxTetes) // Fin de partie
        {
            printf("Tour terminé.\n");
        }
        sem_post(&sem_GJ); // Libère le MJ pour faire jouer les autres joueurs/robots
        sem_wait(&sem_thread[index]); // Le thread attend après avoir posé la carte
        /* Fin d'une Tour */
    } while (Tour <= (10 * MAX_MANCHE )|| maxTetes); // 10 cartes non jouées
    // FIN DE PARTIE
    // Si le joueur se déconnecte, fermez le socket et terminez le thread
    printf(" JEU TERMINE !\n");
    pthread_exit(NULL);
}

/* Démarrage du socket MJ | Renvoie un descripteur du socket MJ */
int initMJ() {
    // Création du socket MJ
    int mj_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (mj_socket == -1) {
        perror("[MJ]: Echec de la création du socket: %s\n");
        exit(EXIT_FAILURE);
    }
    printf("[MJ]: Socket du maitre du jeu créé\n");

    // Paramètres du Maitre du jeu
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET; // Famille de protocole(AF_INET pour TCP/IP)
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Adresse du Maitre du jeu
    serv_addr.sin_port = htons(PORT); // Port à contacter

    // Attachement à un point de communication défini par une adresse et un port,
    int binResult = bind(mj_socket, (struct sockaddr *) &serv_addr, sizeof serv_addr);
    if (binResult == -1) {
        perror("[MJ]: Echec d'attachement du socket: \n");
        exit(EXIT_FAILURE);
    }


    int listenResult = listen(mj_socket, JoueurCount);
    if (listenResult == -1) {
        perror("[MJ]: Echec de la mise en écoute du socket: \n");
        exit(EXIT_FAILURE);
    }
    printf("[MJ]: En écoute sur le port %d\n", PORT);
    return mj_socket;
}

/* Attente d'une connexion joueur au maitre du jeu socket */
void attenteJoueurs(int *mj_socket) {
    int nbRobots;
    int nbJoueurs;

    // Demande le nombre de joueurs humains
    printf("Veuillez choisir le nombre de cnxJoueurH. Le nombre de cnxJoueurH : min 1 et max %d\n", MAX_JOUEURS);
    while (scanf("%d", &nbJoueurs) != 1 || (nbJoueurs < 1 || nbJoueurs > MAX_JOUEURS)) {
        clean_saisieClavier();
        printf("Saisie incorrecte, réessayez\n");
        if (nbJoueurs == 0)
            printf("Le nombre de cnxJoueurH : min 1 et max %d\n", MAX_JOUEURS);
    }
    if (nbJoueurs == 1)
        nbRobots = 1;
    else
        nbRobots = 0;

    //Demande à l'utilisateur le nombre de robots souhaité
    printf("Veuillez choisir le nombre des bots : \n");
    while (scanf("%d", &RobotCount) != 1 || (RobotCount < 0 || RobotCount > (MAX_JOUEURS - nbJoueurs))) {
        clean_saisieClavier();
        printf("Saisie incorrecte, réessayez\n");
        printf("Le nombre de robots : min %d et max %d\n", 0, MAX_JOUEURS - nbJoueurs);
    }

    //Initialisation
    nbRobots += RobotCount;
    RobotCount = 0;

    //Initialisation de sémaphore
    sem_init(&sem_thread[JoueurCount + RobotCount], 0, 0);
    int i;
    sem_getvalue(&sem_thread[JoueurCount + RobotCount], &i);

    printf("[MJ] En attente de la connexion des joueurs pour commencer.\n");
    while (JoueurCount < nbJoueurs) {
        // Paramètres du joueur
        struct sockaddr_in joueur;
        int addr_size = sizeof(joueur);
        printf("[MJ] En attente du joueur n°%d\n", JoueurCount + 1);

        int joueur_socket = accept(*mj_socket, (struct sockaddr *) &joueur, (socklen_t *) &addr_size);
        if (joueur_socket == -1) {
            perror("Erreur de connexion du joueur\n");
        }
        printf("[MJ] Joueur n°%d accepté!\n", JoueurCount+1);

        // Recherchez un emplacement libre dans le tableau des cnxJoueurH
        while (cnxJoueurH[JoueurCount].socket != 0) {
            printf("Valeur du socket du joueur n° %d déja pris = %d\n", cnxJoueurH[JoueurCount].id,
                   cnxJoueurH[JoueurCount].socket);
            JoueurCount = (JoueurCount + 1) % MAX_JOUEURS;
        }

        //stocker les informations du joueur dans le tableau
        cnxJoueurH[JoueurCount].socket = joueur_socket;
        cnxJoueurH[JoueurCount].id = JoueurCount;


        // Créez un thread dédié pour gérer le joueur
        if (pthread_create(&threads[JoueurCount], NULL, GestionConnexionHumains, &cnxJoueurH[JoueurCount]) != 0) {
            perror("Erreur lors de la création du thread joueur\n");
            close(joueur_socket);
            cnxJoueurH[JoueurCount].socket = 0;
        }

        // Avancez à l'emplacement suivant dans le tableau des cnxJoueurH
        JoueurCount = (JoueurCount + 1);
    }
    printf("[MJ]: En attente de la connexion des robots pour commencer.\n");
    int idRobot = JoueurCount-1;
    while (RobotCount < nbRobots) {
        printf("[MJ]: En attente du robot n°%d\n", idRobot);
        idRobot++;
        if(pthread_create(&threads[idRobot], NULL, GestionConnexionRobots, &idRobot) != 0)
            perror("Erreur lors de la création du thread robot\n");
        RobotCount += 1;
    }


    if (JoueurCount == nbJoueurs && RobotCount == nbRobots) {
        close(*mj_socket); // Arrête l'attente de connexion
        printf("[MJ] Le nombre des joueurs total est : %d, dont %d humain(s) et %d robot(s)\n",
               JoueurCount + RobotCount, JoueurCount, RobotCount);
    }

}

int main() {
    char buffer[MAX_JOUEURS];
    int mj_socket = initMJ(); //  Maitre du jeu
    attenteJoueurs(&mj_socket); // Attente des cnxJoueurH
    srand(time(NULL));
/* Fonction déroulement d'une partie */
    sem_init(&sem_GJ, 0, 0);
    printf("[MJ]: Prépare la partie...\n");
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "[MJ]: Prépare la partie...\n");
    logStats(stats);
    Paquet p;
    fillPaquet(&p);
    melangePaquet(&p);
    printf("[MJ]: Distribue les cartes...\n");
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "[MJ]: Distribue les cartes...\n");
    logStats(stats);
    distribuerCartes(&p, mains, JoueurCount + RobotCount, 10);
    for (int i = 0; i < JoueurCount + RobotCount; i++) {
        sem_post(&sem_thread[i]); // débloque tous les threads pour le début de la Tour
    }
    /*           SEM MJ    1          */
    semaphore_wait(&sem_GJ, JoueurCount + RobotCount); // bloquer le MJ
    /*           SEM MJ    1        */
    printf("[MJ]: Prépare le plateau...\n");
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "[MJ]: Prépare le plateau...\n");
    logStats(stats);
    initTable(&plateauDuJeu);
    printf("[MJ]: Mets les cartes sur le plateau...\n");
    bzero(stats, sizeof(stats));
    snprintf(stats, sizeof(stats), "[MJ]: Mets les cartes sur le plateau...\n");
    logStats(stats);
    remplirPlateau(&p, &plateauDuJeu);
    do {
        bzero(buffer, BUFFER_SIZE);
        printf("[MJ]: Attends le choix de cartes des joueurs...\n");
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "[MJ]: Attends le choix de cartes des joueurs...\n");
        logStats(stats);
        for (int i = 0; i < JoueurCount + RobotCount; i++) {
            sem_post(&sem_thread[i]); // débloque tous les threads
        }
        /*           SEM MJ   2           */
        semaphore_wait(&sem_GJ, JoueurCount + RobotCount); // bloque le MJ
        /*           SEM MJ   2           */
        // il est débloqué
        triCroissantP(tabCartes_c, JoueurCount + RobotCount);
        for (int i = 0; i < JoueurCount + RobotCount; ++i) {
            int indice_th = tabCartes_c[i].ind;
            sem_post(&sem_thread[indice_th]); // débloque les threads en ordre croissant
            printf("[MJ] Le joueur %d joue en %d sa carte\n", indice_th + 1, i + 1);
            bzero(stats, sizeof(stats));
            snprintf(stats, sizeof(stats), "[MJ] Le joueur %d joue en %d sa carte\n", indice_th + 1, i + 1);
            logStats(stats);
            sem_wait(&sem_GJ); // se bloque lui-même
            // Fait jouer les autres joueurs
        }
        // incrémente le nombre de tour
        Tour += 1;
    } while (Tour <= (10 * MAX_MANCHE) || maxTetes);

    for (int i = 0; i < JoueurCount + RobotCount; ++i) { // Attente fermeture des threads
        pthread_join(threads[i], NULL);
        printf("[MJ]: Le thread %d est bien arrêtée\n", i);
        bzero(stats, sizeof(stats));
        snprintf(stats, sizeof(stats), "[MJ]: Le thread %d est bien arrêtée\n", i);
        logStats(stats);
        close(cnxJoueurH[i].socket);
    }
    return 0;
}