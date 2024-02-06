
#include <time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "joueurHumain.h"

#define BUFFER_SIZE 500 // Taille du buffer de lecture

void panic(const char *msg) {
    perror(msg);
    exit(1);
}

int initJH(char *host, int port) {
    int descClient;
    struct sockaddr_in addrServ;
    struct sockaddr_in addrClient;
    struct hostent *hote = gethostbyname(host);
    //struct hostent *hote = gethostbyname(argv[1]);
    if (hote == NULL)
        panic("Serveur not found");

    //obtention d'un desc socket valide
    descClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //nommage du socket client tel qu'il le serait par défaut
    addrClient.sin_family = AF_INET;
    addrClient.sin_addr.s_addr = INADDR_ANY; //récupère l'adresse de la machine locale
    addrClient.sin_port = htons(port); //affecte le premier numéro de port libre
    bind(descClient, (struct sockaddr *) &addrClient, (sizeof(struct sockaddr_in)));

    //nom du socket du serveur
    addrServ.sin_family = AF_INET;
    addrServ.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *) (hote->h_addr_list[0])));
    addrServ.sin_port = htons(port);

    if (connect(descClient, (struct sockaddr *) &addrServ, (sizeof(struct sockaddr_in))) == -1)
        panic("Connexion ratée ou trop de cnxJoueurH\n");
    printf("Connexion réussite, attente réponse serveur...\n");

    return descClient;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    char buf[BUFFER_SIZE]; // buffer de lecture
    char intro[1000]; // buffer d'intro
    printf("Veuillez choisir l'IP du serveur:\n");
    char host[20];
    scanf("%s", &host);
    printf("Veuillez choisir le port du serveur:\n");
    int port;
    scanf("%d",&port);


    int socketClient = initJH("localhost", 8087);
    bool sortie = false;
    bool placement_possible = false;
    int rang, reponse, indice_Carte;
//
    bzero(intro, sizeof(intro));
    recv(socketClient, intro, sizeof(intro), 0); // Recevoir msg bienvenue
    printf("%s", intro);
    bzero(buf, BUFFER_SIZE);
    recv(socketClient, buf, BUFFER_SIZE, 0); // Pour recevoir le plateau
    printf("%s", buf);
    while (!sortie) {
        bzero(buf, BUFFER_SIZE);
        recv(socketClient, buf, BUFFER_SIZE, 0); // Recevoir msg debut Tour
        printf("%s", buf);
        bzero(buf, BUFFER_SIZE);
        recv(socketClient, buf, BUFFER_SIZE, 0); // Recevoir main joueur
        printf("%s", buf);
        bzero(buf, BUFFER_SIZE);
        recv(socketClient, buf, BUFFER_SIZE, 0); // Reçoit le msg "choisir carte"
        printf("%s", buf);
        scanf("%d", &indice_Carte);
        while (indice_Carte < 1 || indice_Carte > 10) {
            printf("ERREUR: valeur vide, réessayez\n");
            scanf("%d", &indice_Carte);
        }
        sprintf(buf, "%d", indice_Carte);
        send(socketClient, buf, BUFFER_SIZE, 0); //envoi de la carte choisi

        while (!placement_possible) {
            bzero(buf, BUFFER_SIZE);
            recv(socketClient, buf, BUFFER_SIZE, 0); // reçoit la maj du plateau
            printf("%s", buf);
            bzero(buf, BUFFER_SIZE);
            recv(socketClient, buf, BUFFER_SIZE, 0); // Reçoit le message pour choisir la rangee
            printf("%s", buf);
            bzero(buf, BUFFER_SIZE);
            scanf("%d", &rang); // entre la rangée
            while (rang < 1 || rang > 4) {
                printf("ERREUR: valeur vide, réessayez\n");
                scanf("%d", &rang);
            }
            sprintf(buf, "%d", rang);
            send(socketClient, buf, BUFFER_SIZE, 0); //envoi de la rangée choisie
            bzero(buf, BUFFER_SIZE);
            recv(socketClient, buf, BUFFER_SIZE, 0); // attends le feu vert pour sortir de la boucle
            reponse = atoi(buf);
            if (reponse == 1) {

                placement_possible = true; // rangee correct
            } else {
                placement_possible = false; // rangee incorrect
            }
        }
        placement_possible = false; // réinitialisation

        bzero(buf, BUFFER_SIZE);
        recv(socketClient, buf, BUFFER_SIZE, 0); // recevoir score et msg fin de partie
        printf("%s", buf);

        if (strstr(buf, "Manche terminée.")) // Si le serveur envoi "exit"
            sortie = true;  // en fin de partie
    }
    //quitter proprement
    close(socketClient);
    exit(0);
}