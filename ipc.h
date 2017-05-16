//###########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################
#include "socket.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

int initSemaphore(key_t cle);
void up(int num_sem);
void down(int num_sem);
int sem_destroy();
void start_read();
void stop_read();


Memoire* creerEtAttacherMP(key_t);

Memoire* lireEtAttacherMP(key_t);

void detacherMP(void*);

void detruireMP();

int* creerAttacherRc(key_t); 

int* attacherRc(key_t);