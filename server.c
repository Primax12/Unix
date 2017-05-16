//###########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################
#include "server.h"



void alarmHandler(){
	printf(" le serveur a trop attendu les joueurs !\n");
}

static char buffer[256];

int main(int argc, char* argv[]){
	int port,serverSocket, n ,i, select_res ;
	Player players[4];
	int nbClient = 0 ;
	int nbMaxClient = 4 ;
	fd_set fds;
	key_t clef;
	int semaphoreId;
	Memoire* memoire;
	Carte cartes[60];
	if( argc != 2 ){
		fprintf(stderr,"Usage: %s port\n",argv[0]);
		exit(1);
	}
	
	//port de serveur
	port = atoi(argv[1]);
	
	serverSocket = initialiserServer(port,players);
	fprintf(stderr,"Le serveur attend les demandes de connections \n");
	listen(serverSocket,4);
	signal(SIGALRM,alarmHandler);
	
	while(nbClient < nbMaxClient){
		printf("nombre des joueurs : %d \n ",nbClient); 
		
		//initialiser le select
		FD_ZERO(&fds);
		//j'ajoute le socket Master dans fds
		FD_SET(serverSocket, &fds);

		//le max_fd initiale c'est le serverSocket
		int max_fd = serverSocket;
		//ajouter les fds des clients si ils sont crées
		for(i=0;i<4;i++){
			//les players son initialiser à 0 dans l initialiserSocket
			if(players[i].socket != 0){
				int fd = players[i].socket;
				FD_SET(fd,&fds);
				//si il est le max_fd
				if(fd>max_fd){
					max_fd = fd;
				}
			}
		}
		
		//j attend que qqchose me soit transmit sur un de mes fds
		if ((select_res = select(max_fd+1, &fds, NULL, NULL, NULL)) > 0){
			//si c'est sur le serverSocket c' esst pour la connection
			if(FD_ISSET(serverSocket,&fds)){
				int newSocket = acceptConnexion(serverSocket);
				//attendre son nom our finir son inscription
				fd_set fd;
				FD_ZERO(&fd);
				FD_SET(newSocket, &fd);
				if((select_res =  select(newSocket+1,&fd,NULL,NULL,NULL)) > 0){
					bzero(buffer,256);
					if((n=read(newSocket,buffer,256))  > 0){
						// afficher le message du client et tester si il se desinscrit
						printf("nom du client  : \n %s \n",buffer);	
						if(strcmp(buffer,CODEDESINSCRIPTION)== 0){
							continue;
						}
					}
				}else{
					//si l'alarme lui a pris sans avoir reçu son nom on lui dit qu'il n'est pas inscrit'
					if(errno ==  EINTR){
						//write à l'utilisateur que il n'est pas inscrit si il y a au moins 2 inscrit on commencera la partie
						printf(" le timeout lors d'une inscription , cette inscription est ignoré  \n");
						envoyerMessage(newSocket,"vous avez été lent !veuillez quitter et essayer de se reinscrire \n");
						nbMaxClient =2;
						continue;
					}
				}
				//ajouter le joueur
				Player player;
				strcpy(player.name,buffer);
				player.socket = newSocket;
				
				players[nbClient] = player;
				nbClient++;
				if(nbClient == 1){
					alarm(10);
				}	
			}
			//si c'est une autre message qui n'est pas de connection sur les sockets des clients
			for(i=0;i<nbClient;i++){
				
				if(FD_ISSET(players[i].socket,&fds)){
					int socket = players[i].socket;
					bzero(buffer,256);
					if((n=read(socket,buffer,256))  > 0){
						// afficher le message du client a tester si il se desinscrit
						printf("message du client  %s: \n %s \n",players[i].name ,buffer);
						if(strcmp(buffer,CODEDESINSCRIPTION)==0){
							players[i] = players[nbClient];
							nbClient--;
							//pour regarder si ce joueur qu'on vient d'y placer.
							i--;
						}		
					}
				}
			}

		}else{
                        // si mon select est interrompu par l'alarm  
			if(errno == EINTR){
				printf("----\n");
				nbMaxClient = 2;
			}
		}
	}
	
	 //fin d' inscription
	// creer et attacher à la memoire partage
	SYS((clef = ftok("server.c",2)));
	memoire = creerEtAttacherMP(clef);
	semaphoreId=initSemaphore(clef);
    // je communique les clients la fin d'inscription et j'a joute leur nom dans la memoire partage
	down(semaphoreId);
	printf("pass1\n");
	memoire->nbJoueurs = nbClient;
	up(semaphoreId);
	printf("pass2\n");
	for(n=0;n<nbClient;n++){
		//les envoyer lecode de fin d'inscription
		envoyerMessage(players[n].socket,CODEDEBUTJEU);
		
		down(semaphoreId);
		strcpy(memoire->joueurs[n],players[n].name);
		up(semaphoreId);
	}	
	
	//initialiser les 60 cartes
	initCartes(cartes);
	
	//distribuer de cartes
	printf("distribution de carte\n");
	i=0;
	while(i < 60){
		for(n=0;n<nbClient;n++){
			envoyerCarte(players[n].socket,&(cartes[i]));
			i++;
		}	
	}

	//envoyer une carte.couleur ="fin" pour indiquer la fin de distribution
	for(n=0;n<nbClient;n++){
		Carte carte ;
		strcpy(carte.couleur,"fin");
		envoyerCarte(players[n].socket,&carte);
	}
	printf("debut les ecarts\n");
	//ecarts A faire	
	Carte ecarts[nbClient][5];
	// envoyer aux clients qu'ils peuvent écarter leurs cartes
	// et recevoir leurs cartes écartes
	Carte bufferCarte;
	for(n=0;n<nbClient;n++){
		printf("recevoir les ecarts de %s \n",players[n].name);
		envoyerMessage(players[n].socket,CODEDEBUTECART);
		for(i=0;i<5;i++){
			printf("carte %d\n",i);
			bzero(&bufferCarte,sizeof(Carte));

			open(players[n].socket);
			if((n=read(players[n].socket,&bufferCarte,sizeof(Carte)))  > 0){
				Carte carte = bufferCarte;
				ecarts[n][i] = carte;
				printf(" carte recu size %d \n",carte.valeur);
			}else{
				printf("erreur de read n = %d \n",n);
				printf("%s\n",strerror(errno));
				exit(1);
			}
			close(players[n].socket);
		}
	}
	printf("fini l'ecart \n");


	printf("le debut du jeux \n");
	while(1){
                // le debut du jeu      
	}
	detacherMP(memoire);

}

// pour configurer le fds dans le select
int configurerSelect(int serverSocket,Player* players , fd_set* ptrFds){
	fd_set fds = *ptrFds;
	FD_ZERO(&fds);
	//j'ajoute le socket Master dans fds
	FD_SET(serverSocket, &fds);

	//le max_fd initiale c'est le serverSocket
	int max_fd = serverSocket;
	int i;
	//ajouter les fds des clients si ils sont crées
	for(i=0;i<4;i++){
		//les players son initialiser à 0 dans l initialiserSocket
		if(players[i].socket != 0){
			int fd = players[i].socket;
			FD_SET(fd,&fds);
			//si il est le max_fd
			if(fd>max_fd){
				max_fd = fd;
			}
		}
	}
	return max_fd;	
}

void initCartes(Carte* cartes){
	int i, j,nb=0;
	for (i=0; i<4; i++){
		char couleur[10];
		if(i==0){
			strcpy(couleur,"coeur");
		}
		if(i==1){
			strcpy(couleur,"carreau");
		}
		if(i==2){
			strcpy(couleur,"trefle");
		}
		if(i==3){
			strcpy(couleur,"pique");
		}
		
		for(j=0; j<10; j++){
			Carte c;
			strcpy(c.couleur,couleur);
			c.valeur = (j+1);
			c.points = 0;
			cartes[nb] = c;
			nb++;
		}
	}

	for(i=0; i<20; i++){
		Carte c;
		strcpy(c.couleur,"payoo");
		c.valeur = (i + 1);
		c.points = (i+1);
		cartes[nb] = c;
		nb++;
	}

}
