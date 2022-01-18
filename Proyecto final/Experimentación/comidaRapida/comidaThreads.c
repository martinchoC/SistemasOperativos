#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h> 

#define CANT_MESAS_LIMPIAS 30
#define CANT_MESAS_SUCIAS 0 
#define CANT_COCINEROS 3
#define CANT_CAMAREROS 1
#define CANT_LIMPIADORES 1
#define CANT_CLIENTES 50
#define CAPACIDAD_COLA 10

sem_t semMesaLimpia, semMesaSucia, semCamarero, semCola, semDisponible, semComidaLista;

void* cliente(void* nombre);
void* camarero();
void* limpiador();
void* cocinero(void* nombre);

void main() {
    srand (time(NULL));
    int i; //para ciclar en la creación de hilos
    pthread_t hilosCocineros[CANT_COCINEROS]; //un hilo para cada cocinero
    pthread_t hilosCamareros[CANT_CAMAREROS]; //un hilo para cada camarero
    pthread_t hilosLimpiadores[CANT_LIMPIADORES]; //un hilo para cada limpiador
    pthread_t hilosclientes[CANT_CLIENTES]; //un hilo para cada cliente

    //inicialización de semáforos
    sem_init(&semMesaLimpia, 0, CANT_MESAS_LIMPIAS);
    sem_init(&semMesaSucia, 0, 0);
    sem_init(&semCamarero, 0, 0);
    sem_init(&semCola, 0, CAPACIDAD_COLA);
    sem_init(&semDisponible, 0, 0);
    sem_init(&semComidaLista, 0, 0);
    
    for(i = 0; i < CANT_CAMAREROS; i++) 
    {
        pthread_create(&hilosCamareros[i], NULL, camarero, &i);
    }

    for (i = 0; i < CANT_COCINEROS; i++)
    {
        pthread_create(&hilosCocineros[i], NULL, cocinero, &i);
        sleep(rand() % CANT_COCINEROS + 1);
    }

    for(i = 0; i < CANT_LIMPIADORES; i++) 
    {
        pthread_create(&hilosLimpiadores[i], NULL, limpiador, &i);
    }

    for (i = 0; i < CANT_CLIENTES; i++)
    {
        pthread_create(&hilosclientes[i], NULL, cliente, &i);
        sleep(rand() % 3 + 1);
    }

    for(i=0; i<CANT_CLIENTES; i++)
    {
		pthread_join(hilosclientes[i], NULL);
    }

    for (i = 0; i < CANT_COCINEROS; i++)
    {
        pthread_join(hilosCocineros[i], NULL);
    }

    for(i = 0; i < CANT_LIMPIADORES; i++)
    {
		pthread_join(hilosLimpiadores[i], NULL);
    }

    for(i=0; i<CANT_CAMAREROS; i++)
	{
    	pthread_join(hilosCamareros[i], NULL);
    }

}

void* cliente(void* nombre) {
    int nroCliente = *((int *) nombre);
    while(1) {
        sem_wait(&semMesaLimpia);
        printf("El cliente %d se sienta en una mesa limpia.\n", nroCliente+1);
        sleep(1);
        printf("El cliente %d pide comida.\n", nroCliente+1);
        sleep(1);
        sem_post(&semCamarero);
        sem_wait(&semComidaLista);
        printf("Comida servida al al cliente %d.\n",nroCliente+1);
        sleep(1);
        printf("El cliente %d está comiendo.\n", nroCliente+1);
        sleep(1);
        printf("El cliente %d se retira del restaurant.\n", nroCliente+1);
        sem_post(&semMesaSucia);
        pthread_exit(0);
    }
}

void* camarero() {
    while (1)
    {
        sem_wait(&semCamarero);
        printf("El camarero busca una comida\n");
        sleep(1);
        sem_wait(&semDisponible);
        sem_post(&semCola);
        printf("El camarero lleva una comida\n");
        sleep(1);
        sem_post(&semComidaLista);
    }
    pthread_exit(0);
}

void* limpiador() {
    while (1)
    {
        sem_wait(&semMesaSucia);
        sleep(rand() % 3 + 1);
        printf("El limpiador está limpiando una mesa sucia.\n");
        sleep(rand() % 3 + 1);
        printf("Hay una mesa limpia para ser usada.\n");
        sleep(1);
        sem_post(&semMesaLimpia);
    }
    pthread_exit(0);
}

void* cocinero(void* nombre) {
    int nroCocinero = *((int *) nombre);
    while (1)
    {
        sem_wait(&semCola);
        printf("El cocinero %d va a cocinar una porción de comida.\n",nroCocinero+1);
        sleep(1);
        printf("El cocinero %d está cocinando...\n",nroCocinero+1);
        sleep(1);
        printf("Comida agregada a la cola.\n");
        sleep(1);
        sem_post(&semDisponible);
    }
    pthread_exit(0);
}