all: gj jh

gestionnaireDeStats.o : gestionnaireDeStats.c
	gcc -c gestionnaireDeStats.c

Carte.o: Carte.c
	gcc -c Carte.c

jeu.o: jeu.c
	gcc -c jeu.c

joueurRobot.o : joueurRobot.c
	gcc -c joueurRobot.c

joueurHumain.o: joueurHumain.c
	gcc -c joueurHumain.c

gestionnaireDeJeu.o: gestionnaireDeJeu.c
	gcc -c gestionnaireDeJeu.c

gj: gestionnaireDeJeu.o joueurRobot.o jeu.o Carte.o gestionnaireDeStats.o
	gcc -o gj gestionnaireDeJeu.o joueurRobot.o jeu.o Carte.o gestionnaireDeStats.o

jh: joueurHumain.o Carte.o
	gcc -o jh joueurHumain.o Carte.o

clean:
	rm -f *.o gj jh
