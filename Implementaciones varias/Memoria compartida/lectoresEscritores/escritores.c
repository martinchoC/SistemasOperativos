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
		
		sem_wait(&(mcompartida->escritores));
		printf("empiezo a escribir %i\n",getpid());
		fflush(stdout);
		sleep(1);
		printf("termino de escribir %i\n",getpid());
		fflush(stdout);
		sem_post(&(mcompartida->escritores));
		sleep(3);
		
	}	
		
	
	
	return (0);

}





