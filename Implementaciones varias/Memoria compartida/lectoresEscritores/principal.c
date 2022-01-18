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
	int mem = 0;
	//creo segmento de memoria y lo vinculo al proceso padre
	mem = shmget(CLAVE,(sizeof(struct dato)),0666 | IPC_CREAT);
	//chequeo que el segmento de memoria haya sido creado correctamente
	if (mem == -1){
		printf("No consigo Id para memoria compartida \n");
		exit (0);
	}
	//borro la memoria
	shmctl (mem, IPC_RMID, NULL);
	//creo nuevamente la memoria (esto asegura 
	//que se hizo una creacion y se borro lo anterior)
	mem = shmget(CLAVE,(sizeof(struct dato)),0666 | IPC_CREAT);
	if (mem == -1){
		printf("No consigo Id para memoria compartida \n");
		exit (0);
	}
	//asociamos
	mcompartida = (struct dato*) shmat(mem,NULL,0);
	//chequeo que se haya vinculado correctamente
	if (mcompartida == NULL)
	{
		printf("No consigo memoria compartida \n");
		exit (0);
	}
	
	//inicializo los semaforos que compartiran todos los procesos
	sem_init(&(mcompartida->lectores),1,0);
	sem_init(&(mcompartida->escritores),1,1);	
	sem_init(&(mcompartida->habilitar),1,1);

	//creo todos los procesos hijo (20 lectores y 2 escritores).
	//una vez creados no es necesario vincularlos al segmento de memoria ni volver a inicializar
	//los semaforos, ya que por ser procesos emparentados comparten estos vinculos.
	int i=0;
	while (i<20)
	{
		int a=fork();
		if (a==0){
			 int r1 = execlp("./lectores","lectores",NULL);
			if (r1 == -1) 
				return (1);
		} //si falla termina
		else //soy el padre
			i++;
	}
	i=0;
	while (i<2)
	{
		int b=fork();
		if (b==0){
			int q = execlp("./escritores","escritores",NULL);
			if (q == -1) 
				return (1);
		} //si falla termina
		else //soy el padre
			i++;
	}
	return (0);
}

	      
	



