#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>

//PARA COMPILAR....
//gcc lecheColas.c -o lecheColas -lrt -pthread

//PARA VER ESTADO DE COLAS
//ipcs -q

//PARA BORRAR COLAS
//ipcrm --all=msg

struct message {
  long type;
  char body[150];
};
typedef struct message tMensaje;

#define SIZE_MSG sizeof(tMensaje) - sizeof(long)

#define COLA_TABACO 1234
#define COLA_PAPEL 1235
#define COLA_FOSFORO 1236 
#define COLA_AGENTE 1237

#define TIPO_TABACO 1L
#define TIPO_PAPEL 2L
#define TIPO_FOSFORO 3L
#define TIPO_AGENTE 1L

void crearColasLlenar(); //crear colas y poner el agente en su cola
void* tabaco();
void* papel();
void* fosforo();
void* agente();

void main() {

    pthread_t hiloTabaco;
    pthread_t hiloPapel;
    pthread_t hiloFosforo;
    pthread_t hiloAgente;

    crearColasLlenar();
    sleep(1);
    pthread_create(&hiloAgente, NULL, agente, NULL);
    sleep(1);
    pthread_create(&hiloTabaco, NULL, tabaco, NULL);
    pthread_create(&hiloPapel, NULL, papel, NULL);
    pthread_create(&hiloFosforo, NULL, fosforo, NULL);

    pthread_join(hiloAgente, NULL);
    pthread_join(hiloTabaco, NULL);
    pthread_join(hiloPapel, NULL);
    pthread_join(hiloFosforo, NULL);

}

void crearColasLlenar() {
    
    int colaTabaco = msgget((key_t)COLA_TABACO, 0666 | IPC_CREAT);
    if (colaTabaco < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
    
    int colaPapel = msgget((key_t)COLA_PAPEL, 0666 | IPC_CREAT);
    if (colaPapel < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaFosforo = msgget((key_t)COLA_FOSFORO, 0666 | IPC_CREAT);
    if (colaFosforo < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaAgente = msgget((key_t)COLA_AGENTE, 0666 | IPC_CREAT);
    if (colaAgente < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
    tMensaje msjAgente;
    msjAgente.type = TIPO_AGENTE;
    msjAgente.body[0] = 'A';
    printf("Agente %s agregado a la cola.\n",msjAgente.body);
    msgsnd(colaAgente, &msjAgente, SIZE_MSG, IPC_NOWAIT);
}

void* tabaco()
{
    tMensaje msjPapel, msjFosforo, msjAgente;
    int colaPapel = msgget((key_t)COLA_PAPEL, 0666 | IPC_CREAT);
    int colaFosforo = msgget((key_t)COLA_FOSFORO, 0666 | IPC_CREAT);
	while(1)
	{
        if (msgrcv(colaPapel,&msjPapel,SIZE_MSG,TIPO_PAPEL,IPC_NOWAIT) > 0)
		{
			if (msgrcv(colaFosforo,&msjFosforo,SIZE_MSG,TIPO_FOSFORO,IPC_NOWAIT) > 0)
			{
				printf("El fumador con tabaco fuma\n");
			}
			else
			{
				msgsnd(colaPapel,&msjPapel,SIZE_MSG,IPC_NOWAIT);
			}			
		}
	}
	pthread_exit(0);
}

void* papel()
{
    tMensaje msjTabaco, msjFosforo, msjAgente;
    int colaTabaco = msgget((key_t)COLA_TABACO, 0666 | IPC_CREAT);
    int colaFosforo = msgget((key_t)COLA_FOSFORO, 0666 | IPC_CREAT);
	while(1)
	{
		if (msgrcv(colaTabaco,&msjTabaco,SIZE_MSG,TIPO_TABACO,IPC_NOWAIT) > 0)
		{
			if (msgrcv(colaFosforo,&msjFosforo,SIZE_MSG,TIPO_FOSFORO,IPC_NOWAIT) > 0)
			{
				printf("El fumador con papel fuma\n");
			}
			else
			{
				msgsnd(colaTabaco,&msjTabaco,SIZE_MSG,IPC_NOWAIT);
			}			
		}
	}
	pthread_exit(0);
}

void* fosforo()
{
    tMensaje msjPapel, msjTabaco, msjAgente;
    int colaTabaco = msgget((key_t)COLA_TABACO, 0666 | IPC_CREAT);
    int colaPapel = msgget((key_t)COLA_PAPEL, 0666 | IPC_CREAT);
	while(1)
	{
		if (msgrcv(colaTabaco,&msjTabaco,SIZE_MSG,TIPO_TABACO,IPC_NOWAIT) > 0)
		{
			if (msgrcv(colaPapel,&msjPapel,SIZE_MSG,TIPO_PAPEL,IPC_NOWAIT) > 0)
			{
				printf("El fumador con el fosforo fuma\n");
			}
			else
			{
				msgsnd(colaTabaco,&msjTabaco,SIZE_MSG,IPC_NOWAIT);
			}			
		}
	}
	pthread_exit(0);
}

void* agente()
{
	int num;
    tMensaje msjAgente, msjTabaco, msjPapel, msjFosforo;
    int colaAgente = msgget((key_t)COLA_AGENTE, 0666 | IPC_CREAT);
    int colaTabaco = msgget((key_t)COLA_TABACO, 0666 | IPC_CREAT);
    int colaPapel = msgget((key_t)COLA_PAPEL, 0666 | IPC_CREAT);
    int colaFosforo = msgget((key_t)COLA_FOSFORO, 0666 | IPC_CREAT);
	while(1)
	{
        srand(time(NULL));
        msgrcv(colaAgente,&msjAgente,SIZE_MSG,TIPO_AGENTE,0);
		num = rand() % 3 + 1;
        printf("CASO %d\n",num);
		switch (num)
		{
			case 1: //fuma el que tiene tabaco
				msgsnd(colaPapel,&msjPapel,SIZE_MSG,IPC_NOWAIT);
				msgsnd(colaFosforo,&msjFosforo,SIZE_MSG,IPC_NOWAIT);
				break;
			case 2: //fuma el que tiene papel
				msgsnd(colaTabaco,&msjTabaco,SIZE_MSG,IPC_NOWAIT);
				msgsnd(colaFosforo,&msjFosforo,SIZE_MSG,IPC_NOWAIT);
				break;
			case 3: //fuma el que tiene fosforo
				msgsnd(colaTabaco,&msjTabaco,SIZE_MSG,IPC_NOWAIT);
				msgsnd(colaPapel,&msjPapel,SIZE_MSG,IPC_NOWAIT);
				break;
		}
        sleep(3);
		msgsnd(colaAgente,&msjAgente,SIZE_MSG,IPC_NOWAIT);
	}
	pthread_exit(0);
}
