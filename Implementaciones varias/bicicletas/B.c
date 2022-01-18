#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

// Secuencia ABCDE...

sem_t sOP1, sOP2, sOP3, sMontador, sRueda, sCuadro, sManillar;

void* op1()
{
	while(1)
	{
		sem_wait(&sOP1);
		sleep(1);
		printf("Operario 1 fabricó rueda 1\n");
		sem_post(&sRueda);
		sleep(1);
		printf("Operario 1 fabricó rueda 2\n");
		sem_post(&sRueda);
	}
	pthread_exit(0);
}

void* op2()
{
	while(1)
	{
		sem_wait(&sOP2);
		sleep(1);
		printf("Operario 2 fabricó un manillar\n");
		sem_post(&sManillar);
	}
	pthread_exit(0);
}

void* op3()
{
	while(1)
	{
		sem_wait(&sOP3);
		sleep(1);
		printf("Operario 3 fabricó un cuadro\n");
		sem_post(&sCuadro);
	}
	pthread_exit(0);
}

void* montador()
{
	while(1)
	{
		sem_wait(&sMontador);
		sem_wait(&sRueda);
		sem_wait(&sRueda);
		sem_wait(&sManillar);
		sem_wait(&sCuadro);
		printf("El montador toma dos ruedas, un manillar y un cuadro y fabrica una bicicleta\n");
		sleep(1);
		sem_post(&sMontador);
		sem_post(&sOP1);
		sem_post(&sOP2);
		sem_post(&sOP3);

	}
	pthread_exit(0);
}

int main()
{
	sem_init(&sOP1, 0, 1);
	sem_init(&sOP2, 0, 1);
	sem_init(&sOP3, 0, 1);
    sem_init(&sMontador, 0, 1);
	sem_init(&sRueda, 0, 0);
	sem_init(&sCuadro, 0, 0);
	sem_init(&sManillar, 0, 0);
	
	pthread_t th1, th2, th3, th4, th5;
	
	pthread_create(&th1, NULL, op1, NULL);
	pthread_create(&th2, NULL, op2, NULL);
	pthread_create(&th3, NULL, op3, NULL);
    pthread_create(&th4, NULL, montador, NULL);
	
	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);
    pthread_join(th4, NULL);
	
	exit(0);
}