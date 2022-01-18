#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

//problema de los fumadores

sem_t sTabaco, sPapel, sFosforo, sAgente;

void* tabaco()
{
	while(1)
	{
        if (sem_trywait(&sPapel) == 0)
		{
			if (sem_trywait(&sFosforo) == 0)
			{
				printf("El fumador con tabaco fuma\n");
			}
			else
			{
				sem_post(&sPapel);
			}			
		}
	}
	pthread_exit(0);
}

void* papel()
{
	while(1)
	{
		if (sem_trywait(&sTabaco) == 0)
		{
			if (sem_trywait(&sFosforo) == 0)
			{
				printf("El fumador con papel fuma\n");
			}
			else
			{
				sem_post(&sTabaco);
			}			
		}
	}
	pthread_exit(0);
}

void* fosforo()
{
	while(1)
	{
		if (sem_trywait(&sTabaco) == 0)
		{
			if (sem_trywait(&sPapel) == 0)
			{
				printf("El fumador con el fosforo fuma\n");
			}
			else
			{
				sem_post(&sTabaco);
			}			
		}
	}
	pthread_exit(0);
}

void* agente()
{
	int num;
	while(1)
	{
		sem_wait(&sAgente);
		srand(time(NULL));
		num = rand() % 3 + 1;
		switch (num)
		{
			case 1: //fuma el que tiene tabaco
				sem_post(&sPapel);
				sem_post(&sFosforo);
				break;
			case 2: //fuma el que tiene papel
				sem_post(&sTabaco);
				sem_post(&sFosforo);
				break;
			case 3: //fuma el que tiene fosforo
				sem_post(&sTabaco);
				sem_post(&sPapel);
				break;
		}
		sleep(1);
		sem_post(&sAgente);
	}
	pthread_exit(0);
}

int main()
{
	sem_init(&sTabaco, 0, 0);
	sem_init(&sPapel, 0, 0);
	sem_init(&sFosforo, 0, 0);
    sem_init(&sAgente, 0, 1);
	
	pthread_t th1, th2, th3, th4;
	
	pthread_create(&th1, NULL, tabaco, NULL);
	pthread_create(&th2, NULL, papel, NULL);
	pthread_create(&th3, NULL, fosforo, NULL);
    pthread_create(&th4, NULL, agente, NULL);
	
	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	pthread_join(th3, NULL);
    pthread_join(th4, NULL);
	
	exit(0);
}