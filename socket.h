//###########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################
#include    <netdb.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <sys/socket.h>
#include    <sys/types.h>
#include    <netinet/in.h>
#include	<signal.h>
#include <sys/select.h>
#include <unistd.h>
 #include <sys/time.h>
#include <errno.h>
#define SYS(call) ((call) == -1) ?(perror(#call ":ERROR"),exit(1)) : 0

#define CODEDESINSCRIPTION "desinscription"
#define CODEDEBUTJEU "debutJeu"
#define CODEDEBUTECART "debutEcart"

typedef struct Player{
	char name[50];
	int socket;
}Player;

typedef struct Carte{
	char couleur[10];
	int valeur;
	int points;
}Carte;

typedef struct Memoire{
	char joueurs[4][100];
	int nbJoueurs;	
}Memoire;

int initialiserServer(int , Player*);
//serverSocket,fds,max_fd retourne le newSocket
int acceptConnexion(int);
void envoyerMessage(int,char*);
void envoyerCarte(int,Carte*);