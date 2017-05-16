###########################
# Projet réseau 
#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
#serie 2
#############################
CC=gcc
CFLAGS=-Wall -Wextra

normal: server client

client: client.o socket.o ipc.o
	$(CC) $(CFLAGS) client.o socket.o ipc.o -o client

server: server.o socket.o ipc.o
	$(CC) $(CFLAGS) server.o  socket.o ipc.o -o server

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

socket.o: socket.h socket.c
	$(CC) $(CFLAGS) -c socket.c

ipc.o: ipc.h ipc.c
	$(CC) $(CFLAGS) -c ipc.c

clean:
	$(RM)  server client