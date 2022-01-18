#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//problema del oso y las abejas

#define CANT_ABEJAS 10
#define CANT_PORCIONES 7

void* abeja(void* nroAbeja);
void* oso();

sem_t sMiel, sTarro, sComer, sAbejaMutex;

void main(){ 
    pthread_t hilosAbejas[CANT_ABEJAS];
    pthread_t hiloOso;
    int i;

    sem_init(&sMiel, 0, 0);
    sem_init(&sTarro, 0, CANT_PORCIONES);
    sem_init(&sComer, 0, 0);
    sem_init(&sAbejaMutex, 0, 1);

    for(i = 0; i < CANT_ABEJAS; i++){
        pthread_create(&hilosAbejas[i], NULL, abeja, &i);
        usleep(100);
    }

    pthread_create(&hiloOso, NULL, oso, NULL);

    for (i = 0; i < CANT_ABEJAS; i++)
    {
        pthread_join(hilosAbejas[i],NULL);
    }

    pthread_join(hiloOso,NULL);
    
}

void* abeja(void* nroAbeja) {
    int abeja = *((int *) nroAbeja);
    while (1)
    {
        srand (time(NULL));
        sem_wait(&sAbejaMutex);
        sem_post(&sMiel);
        sem_wait(&sTarro);
        printf("La abeja %d agrega una porcion al tarro.\n",abeja + 1);
        sleep(1);
        if(sem_trywait(&sTarro) != 0) {
            printf("La abeja %d despierta al oso.\n",abeja + 1);
            sleep(1);
            sem_post(&sComer);
        }
        else
        {
            sem_post(&sTarro);
            sem_post(&sAbejaMutex);
            sleep(rand() % CANT_ABEJAS + 1);
        }
        

    }

}

void* oso() {
    int i;
    while (1)
    {
        sem_wait(&sComer);
        printf("El oso es despertado por una abeja y come la miel.\n");
        for (i = 0; i < CANT_PORCIONES; i++)
        {
            sem_wait(&sMiel);
            sem_post(&sTarro);
        }
        sleep(1);
        printf("El oso se vuelve a dormir hasta tener otro tarro de miel\n");
        sleep(1);
        sem_post(&sAbejaMutex);
    }
    
    
}
