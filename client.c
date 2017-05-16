//##########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################

#include "client.h"
void afficherCartes(Carte* ,int);
void intHandler(){
	printf("je decide de quitter ma parti");
	exit(1);
}
int main(int argc, char* argv[]){
	int port , i,n, select_res;
	int socketClient;
	struct sockaddr_in serverAddr;
	struct hostent *host;
	char buffer[256];
	fd_set fds;
	key_t clef;
	int semaphoreId;
	Memoire* memoire;
	Carte myCards[30];
	int nbCartes =0;

	if( argc != 3 ){ 
		fprintf(stderr,"Usage: %s ip port\n",argv[0]);
		exit(1);
	}
	
	//je créer le socketClient	
	SYS (socketClient = socket(AF_INET,SOCK_STREAM,0));
	port = atoi(argv[2]);

	// le host c'est le server
	if((host = gethostbyname(argv[1]))== NULL){
		perror("no such host");
		exit(1);
	}

	//configurer le serveur Adresss avant de se connecter à cet address
	//padding to the field de l' address...
	bzero((char*)&serverAddr, sizeof(struct sockaddr_in));
	serverAddr.sin_family      = AF_INET;
	//copier l'adresse de host dans l' adresse de server.
	bcopy(host->h_addr,(char*)&serverAddr.sin_addr.s_addr,host->h_length);
	serverAddr.sin_port = htons(port);

	//maintenant se connecter au serveur
	if( connect(socketClient, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0 ){
		perror("impossible de connecter le client au serveur \n");
		exit(1);
	}
	
	printf(" inscription au jeux établi \n");
	printf(" entrez votre nom \n");
	signal(SIGINT,intHandler);	
	while(1){

		//configurer le select
		FD_ZERO(&fds);
		//j'ajoute le socket du client 
		FD_SET(socketClient, &fds);
		FD_SET(0,&fds);
		
		if((select_res =  select(socketClient+1,&fds,NULL,NULL,NULL)) > 0){
			if(FD_ISSET(0,&fds)){
				bzero(buffer,256);
				//lire tant qu' il entre des valeur non vide (EOF ou que \n)
				while(fgets(buffer,256,stdin)== NULL  || strlen(buffer)-1 == 0){ 
					printf("entrez un nom non-vide \n");
				}
				envoyerMessage(socketClient,buffer);
			}else{

				bzero(buffer,256);
				if((n=read(socketClient,buffer,256))  != 0){
					if(strcmp(buffer,CODEDEBUTJEU)==0){
						break;
					}
					// afficher le message du client
					printf("message du serveur  : \n %s \n",buffer);	
				}
			}
		}else{
			if(errno== EINTR){
				// le  joueur quite la partie
				if((n = write(socketClient,CODEDESINSCRIPTION,256)) < 0){
					perror("erreur write to client ko \n");
					exit(1);
				}
				exit(1);
			}
		}
	}
	
	SYS(clef = ftok("server.c",2));
	memoire = lireEtAttacherMP(clef);
	semaphoreId = initSemaphore(clef);
	
	// afficher les joueurs inscrits dele memoire partagée
	start_read();
	printf("joueurs inscrits: \n");
	for(i=0;i<(memoire->nbJoueurs);i++){;
		printf("%s",(memoire->joueurs)[i]);
	}
	stop_read();
	//distribution de carte
	while(1){
		Carte bufferCarte;
		if((select_res =  select(socketClient+1,&fds,NULL,NULL,NULL)) > 0){
			if(FD_ISSET(socketClient,&fds)){
				bzero(&bufferCarte,sizeof(Carte));
				if((n=read(socketClient,&bufferCarte,sizeof(Carte)))  != 0){
						//si la carte renvoyé est null c'est la fin de distribution
					Carte carte = bufferCarte;
					if(strcmp(carte.couleur,"fin")==0){
						break;
					}
					myCards[nbCartes] = carte;
					nbCartes++;
				}
			}
		}
	}	
	afficherCartes(myCards,nbCartes);
	printf("fin de la distribution\n");
	// attendre le code du debut d'ecart
	while(1){
		FD_ZERO(&fds);
		//j'ajoute le socket du client 
		FD_SET(socketClient, &fds);
		FD_SET(0,&fds);

		if((select_res =  select(socketClient+1,&fds,NULL,NULL,NULL)) > 0){
			if(FD_ISSET(socketClient,&fds)){
				bzero(buffer,256);
				if((n=read(socketClient,buffer,256))!= 0){
					
					if(strcmp(buffer,CODEDEBUTECART)==0){
						printf("code debut ecart reçu\n");
						break;
					}
				}
			}
		}
	}
	//maintenant le temps de choisir les 5 cartes à ecartes
	i=0;
	while(i<5){
		printf("entrez le numero correspondante à la carte à écarte \n");
		
		bzero(buffer,256);
				//lire tant qu' il entre des valeur non vide (EOF ou que \n)
		while(fgets(buffer,256,stdin)== NULL  || strlen(buffer)-1 == 0){ 
			printf("entrez un numero non-vide \n");
		}
		int numero = atoi(buffer);
		printf("%d\n",numero);
		if(numero < 0 || numero >= nbCartes){
			printf("mauvais numero oups\n");
			continue;
		}
		envoyerCarte(socketClient,&(myCards[numero]));
		myCards[numero] = myCards[nbCartes];
		nbCartes--;
		i++;
		printf("carte bien écartée\n"); 
	}
	while(1){

	}
}
void afficherCartes(Carte* myCards,int nbCartes){
	int i = 0;
	for(i=0;i<nbCartes;i++){
		printf(" %d ->carte %s :: valeur: %d \n",i,(myCards[i]).couleur,(myCards[i]).valeur);
	}
}
