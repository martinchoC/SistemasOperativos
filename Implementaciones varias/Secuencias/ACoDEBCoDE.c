#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// Secuencia A(CoD)EB(CoD)...

sem_t sA,sB,sCD,sE;

void* tareaA()
{
	while(1)
	{
		sem_wait(&sA);
		printf("A");
		sem_post(&sCD);
	}
	pthread_exit(0);
}

void* tareaB()
{
	while(1)
	{
		sem_wait(&sB);
		printf("B");
		sem_post(&sCD);
	}
	pthread_exit(0);
}

void* tareaC()
{
	while(1)
	{
		sem_wait(&sCD);
		printf("C");
		sem_post(&sE);
	}
	pthread_exit(0);
}

void* tareaD()
{
	while(1)
	{
		sem_wait(&sCD);
		printf("D");
		sem_post(&sE);
	}
	pthread_exit(0);
}

void* tareaE()
{
	while(1)
	{
		sem_wait(&sE);
		printf("E");
		sem_post(&sB);
		sem_wait(&sE);
		printf("E");
		sem_post(&sA);		
	}
	pthread_exit(0);
}

int main()
{
	sem_init(&sA, 0, 1);
	sem_init(&sB, 0, 0);
	sem_init(&sCD, 0, 0);
	sem_init(&sE, 0, 0);
	
	pthread_t th1, th2, th3, th4, th5;
	
	pthread_create(&th1, NULL, tareaA, NULL);
	pthread_create(&th2, NULL, tareaB, NULL);
	pthread_create(&th3, NULL, tareaC, NULL);
	pthread_create(&th4, NULL, tareaD, NULL);
	pthread_create(&th5, NULL, tareaE, NULL);
	
	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);
	pthread_join(th4, NULL);
	pthread_join(th5, NULL);
	
	exit(0);
}