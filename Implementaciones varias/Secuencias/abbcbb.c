#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Secuencia ABBCBB...

sem_t sA,sB,sC;

void* tareaA()
{
	while(1)
	{
		sem_wait(&sA);
		printf("A");
		sem_post(&sB);
		sem_wait(&sA);
		sem_post(&sB);
	}
	pthread_exit(0);
}

void* tareaB()
{
	while(1)
	{
		sem_wait(&sB);
		printf("B");
		sem_post(&sA);
		sem_wait(&sB);
		printf("B");
		sem_post(&sC);
		sem_wait(&sB);
		printf("B");
		sem_post(&sC);
		sem_wait(&sB);
		printf("B");
		sem_post(&sA);
	}
	pthread_exit(0);
}

void* tareaC()
{
	while(1)
	{
		sem_wait(&sC);
		printf("C");
		sem_post(&sB);
		sem_wait(&sC);
		sem_post(&sB);
	}
	pthread_exit(0);
}

int main()
{
	sem_init(&sA, 0, 1);
	sem_init(&sB, 0, 0);
	sem_init(&sC, 0, 0);
	
	pthread_t th1, th2, th3, th4, th5;
	
	pthread_create(&th1, NULL, tareaA, NULL);
	pthread_create(&th2, NULL, tareaB, NULL);
	pthread_create(&th3, NULL, tareaC, NULL);
	
	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);
	
	exit(0);
}