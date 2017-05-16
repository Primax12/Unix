//###########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################

#include "socket.h"
int initialiserServer(int port,Player* players){
	int serverSocket  ;
	struct sockaddr_in serverAddr;
	
	//creer le socket Master
    	if( (serverSocket = socket(AF_INET,SOCK_STREAM,0)) < 0 ){
	   	perror("server - socket");
	        exit(1);
	}
	
	//configurer l' address du serveur
	// mettre le padding à zero
    	bzero((char*)&serverAddr,sizeof(struct sockaddr_in));
    	serverAddr.sin_family      = AF_INET;
   	 serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    	serverAddr.sin_port        = htons(port);

	//lier l addresse au socket Master
    	if( bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0 ){
		   perror("server - bind");
        	exit(1);
    	}
	int i =0;
	for(i = 0;i<4;i++){
		Player player ;
		strcpy(player.name,"");
		player.socket= 0;
		players[i] = player;
	}		
	return serverSocket;
}
int 	acceptConnexion(int serverSocket){
	struct sockaddr_in clientAddr;
	u_int len2 = sizeof(clientAddr);
	int   newSocket = accept(serverSocket,(struct sockaddr *)&clientAddr,&len2);
	printf("Connection from %d %d \n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));	

	return newSocket;
}
void envoyerMessage(int socket, char * msg){
	if((write(socket,msg,256)) < 0){
		perror("erreur write to socket ko \n");
		exit(1);
	}
}

void envoyerCarte(int socket, Carte* carte){
	if((write(socket,carte,sizeof(Carte))) < 0){
		perror("erreur write to socket ko \n");
		exit(1);
	}
}