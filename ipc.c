//###########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################
#include "ipc.h"
int shmid;
int rcid;

int id_sem;
int* rc=0;
key_t clefRc;

int initSemaphore(key_t cle){
    struct sembuf sem_buffer;

    id_sem = semget(cle,2,IPC_CREAT | 0666);

    if(id_sem >=0){
        sem_buffer.sem_op = 1;
        sem_buffer.sem_flg = 0;

        for(sem_buffer.sem_num = 0; sem_buffer.sem_num <2 ; sem_buffer.sem_num++){
            if(semop(id_sem,&sem_buffer,1) == -1){
                sem_destroy(id_sem);
                return -1;
            }
        
        }
        clefRc=ftok("client.c",5);
        rc = creerAttacherRc(clefRc);
        *rc = 0;
   } else{
    rc =attacherRc(clefRc);
   }
   return id_sem;

 }

void up(int num_sem){
    static struct sembuf oper;
    oper.sem_num = num_sem;
    oper.sem_op = 1;
    oper.sem_flg = SEM_UNDO;
    semop(id_sem,&oper,1);
}
void down(int num_sem){
    static struct sembuf oper;
    oper.sem_num = num_sem;
    oper.sem_op -= 1;
    oper.sem_flg = SEM_UNDO;
    semop(id_sem,&oper,1);
}
int sem_destroy(){
    detacherRc(rc);
    return semctl(id_sem,0,IPC_RMID);
}

void start_read(){
    down(0);
    *rc++;
    if(*rc == 1){
        down(1);
    }
    up(0);
}

void stop_read(){
    down(0);
    *rc--;
    if(*rc==0){
        up(1);
    }
    up(0);
}

Memoire* creerEtAttacherMP(key_t clef){
	Memoire* mp=NULL;
	if((shmid =shmget(clef,sizeof(Memoire), IPC_CREAT | 0666)) <0){
		perror("Erreur lors de la création de la mémoire partagée");
		exit(1);
	}

	if((mp = (Memoire *) shmat(shmid, NULL, 0)) == (Memoire*) -1){
		perror("Erreur lors de l'attachement à la mémoire");
		detruireMP();
		exit(1);
	}

	return mp;
}

Memoire* lireEtAttacherMP(key_t clef){
	int identifiantMP;
	Memoire* mp=NULL;

	if((identifiantMP = shmget(clef,0, 0444)) <0){
		perror("La mémoire partagée n'a pas été initialisé (lire et attacher mp)");
		exit(1);
	}

	if((mp=shmat(identifiantMP, NULL, 0)) == (Memoire*) -1){
		perror("Erreur lorsque que l'on attache pour lecture ");
		exit(1);
	}

	return mp;
}


void detacherMP(void* mp){
	if((shmdt(mp)) < 0){
		perror("Erreur lors du détachement de la mémoire partagée");
		exit(1);
	}
}

void detruireMP(){
	if((shmctl(shmid, IPC_RMID, NULL))< 0){
		perror("Erreur déstruction mémoire partagée");
		exit(1);
	}
}

int* creerAttacherRc(key_t clef){
	int *rc;
	if((rcid = shmget(clef, sizeof(int), IPC_CREAT | 0666)) < 0){
		perror("Erreur lors de la création de la rc à la mémoire partagée");
		exit(1);
	}

	if((rc = (int*) shmat(rcid, NULL, 0)) < 0){
		perror("Erreur lors de l'attachement de la rc à la mémoire");
		exit(1);
	}
	return rc;
}

int* attacherRc(key_t clef){
	int *rc;
	if((rcid = shmget(clef, 0, 0)) < 0){
		perror("Erreur lors de la création de la rc à la mémoire partagée");
		exit(1);
	}

	if((rc = (int*) shmat(rcid, NULL, 0)) < 0){
		perror("Erreur lors de l'attachement de la rc à la mémoire");
		exit(1);
	}
	return rc;
}

void detacherRc(void* rc){
	if((shmctl(rcid, IPC_RMID, NULL))< 0){
		perror("Erreur déstruction mémoire partagée");
		exit(1);
	}

	if((shmdt(rc)) < 0){
		perror("Erreur lors du détachement de la mémoire partagée");
		exit(1);
	}
}