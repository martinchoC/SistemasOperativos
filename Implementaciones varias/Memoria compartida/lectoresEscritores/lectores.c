#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define CLAVE 123456
struct dato
{ 
	sem_t lectores;
	sem_t escritores;
	sem_t habilitar;
};

struct dato* mcompartida;


int main(){
	//vinculacion
	int mem = shmget(CLAVE,(sizeof(struct dato)),0666);	
	//chequeo que el segmento de memoria haya sido creado correctamente
	if (mem == -1){
		printf("No consigo Id para memoria compartida \n");
		exit (0);
	}
	//asocio
	mcompartida = (struct dato*) shmat(mem,NULL,0);
	//chequeo que se haya vinculado correctamente
	if (mcompartida == NULL)
	{
		printf("No consigo memoria compartida \n");
		exit (0);
	}
		
		
	while(1){
		
		sem_wait(&(mcompartida->habilitar));
		if (sem_trywait(&(mcompartida->lectores))==-1){ //no pude alocar el recurso lectores
			sem_wait(&(mcompartida->escritores)); //el primer lector bloquea al escritor 		 
			}
		else{ 	//aloque el recurso lectores
			sem_post(&(mcompartida->lectores));
			sleep(1);
			//printf("no soy el primero : %c \n",L[0]);
			//fflush(stdout);
		}
		sem_post(&(mcompartida->lectores));
		sem_post(&(mcompartida->habilitar));
		
		//seccion critica
		fflush(stdout);
		printf("empiezo a leer lector= %i\n", getpid());
		fflush(stdout);		
		sleep(3);

		printf("termino de leer lector= %i\n",getpid());
		fflush(stdout);		
		//fin seccion critica
		
		sem_wait(&(mcompartida->habilitar));
		sem_wait(&(mcompartida->lectores));
		if (sem_trywait(&(mcompartida->lectores))==-1){//quiere decir q no hay lectores
			sem_post(&(mcompartida->escritores));
		}
		else{  //aun qedan lectores
			sem_post(&(mcompartida->lectores));
			//printf("salgo : %c \n",L[0]);
			//fflush(stdout);
			usleep(500);
		}
		sem_post(&(mcompartida->habilitar));
		sleep(8);
	}
	
	return (0);

}




