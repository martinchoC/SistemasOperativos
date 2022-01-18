#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <time.h> 

#define CLAVE 123455
#define CONSUMIDORES 3
#define PRODUCTORES 1

struct dato
{ 
	sem_t habilitar;
	sem_t productos;
	sem_t producir;
	int cantProductos;
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
		if(sem_trywait(&(mcompartida->productos)) == 0) { //si hay productos listos
			//seccion critica
			sleep(rand() % 3 + 1);
			mcompartida->cantProductos = mcompartida->cantProductos - 1;
			printf("Consumidor %i consumió un producto, quedan %d productos\n",getpid(), mcompartida->cantProductos);
			sem_post(&(mcompartida->producir)); //hay un lugar para producir
			//fin seccion critica
		}
		else
		{
			printf("Consumidor %i no puede consumir porque no quedan productos\n", getpid());
			sleep(4);
		}
	}
	
	return (0);

}




