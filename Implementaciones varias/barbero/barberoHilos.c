#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//problema del barbero

#define CANT_SILLAS 10
#define CANT_CLIENTES 50

void* cliente(void* nroCliente);
void* barbero();

sem_t semSillaLibre, semSillaOcupada, semMutexBarbero;

void main(){ 
    pthread_t hilosClientes[CANT_CLIENTES];
    pthread_t hiloBarbero;
    int i;

    sem_init(&semSillaLibre, 0, CANT_SILLAS);
    sem_init(&semSillaOcupada, 0, 0);
    sem_init(&semMutexBarbero, 0, 1);

    pthread_create(&hiloBarbero, NULL, barbero, NULL);

    for(i = 0; i < CANT_CLIENTES; i++){
        pthread_create(&hilosClientes[i], NULL, cliente, &i);
        sleep(1);
    }

    for (i = 0; i < CANT_CLIENTES; i++)
    {
        pthread_join(hilosClientes[i],NULL);

    }

    pthread_join(hiloBarbero,NULL);
    
}

void* cliente(void* nroCliente) {
    int num = *((int *) nroCliente);
    while (1)
    {
        srand (time(NULL));
        if(sem_trywait(&semSillaLibre) == 0) {
            printf("El cliente %d se sienta en la sala de espera.\n",num+1);
            sem_post(&semSillaOcupada);
        }
        else
        {
            printf("Sala de espera llena. El cliente %d vuelve más tarde\n",num+1);
        }
        sleep(rand() % CANT_CLIENTES/2 + 1);
    }
}

void* barbero() {
    int i;
    while (1)
    {
        sem_wait(&semMutexBarbero);
        if (sem_trywait(&semSillaOcupada) == 0)
        {
            sem_post(&semSillaLibre);
            printf("Un cliente se corta el pelo...\n");
            sleep(1);
            printf("Un cliente se retira con el pelo cortado\n");
        }
        else
        {
            printf("El barbero duerme...\n");
        }
        sleep(1);
        sem_post(&semMutexBarbero);
        
        
    }
    
    
}