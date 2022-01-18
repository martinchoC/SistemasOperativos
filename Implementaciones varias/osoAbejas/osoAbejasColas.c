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

//problema del oso y las abejas

//PARA COMPILAR....
//gcc osoAbejasColas.c -o osoAbejas -lrt -pthread

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

#define COLA_MIEL 1234
#define COLA_TARRO 1235
#define COLA_COMER 1236
#define COLA_ABEJA_MUTEX 1237

#define TIPO_MSJ 1L

#define CANT_ABEJAS 10
#define CANT_PORCIONES 7

void crearColas();
void prepararTarro();
void* abeja(void* nroAbeja);
void* oso();

void main(){ 
    pthread_t hilosAbejas[CANT_ABEJAS];
    pthread_t hiloOso;
    int i;

    crearColas();

    prepararTarro();

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

void crearColas() {
    int colaMiel = msgget((key_t)COLA_MIEL, 0666 | IPC_CREAT);
    if (colaMiel < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
    
    int colaTarro = msgget((key_t)COLA_TARRO, 0666 | IPC_CREAT);
    if (colaTarro < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaComer = msgget((key_t)COLA_COMER, 0666 | IPC_CREAT);
    if (colaComer < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    tMensaje msjAbejaMutex;
    int colaAbejaMutex = msgget((key_t)COLA_ABEJA_MUTEX, 0666 | IPC_CREAT);
    if (colaAbejaMutex < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
    msjAbejaMutex.type = TIPO_MSJ;
    msgsnd(colaAbejaMutex,&msjAbejaMutex,SIZE_MSG,IPC_NOWAIT);
    sleep(1);
}

void prepararTarro() {
    int colaTarro = msgget((key_t)COLA_TARRO, 0666 | IPC_CREAT);
    tMensaje msj;
    int i = 1;
    printf("Preparando el tarro de miel...\n");
    while (i <= CANT_PORCIONES)
    {
        msj.type = TIPO_MSJ;
        msgsnd(colaTarro, &msj, SIZE_MSG, IPC_NOWAIT);
        sleep(1);
        i++;
    }
    
}


void* abeja(void* nroAbeja) {
    int colaMiel = msgget((key_t)COLA_MIEL, 0666 | IPC_CREAT);
    int colaTarro = msgget((key_t)COLA_TARRO, 0666 | IPC_CREAT);
    int colaComer = msgget((key_t)COLA_COMER, 0666 | IPC_CREAT);
    int colaAbejaMutex = msgget((key_t)COLA_ABEJA_MUTEX, 0666 | IPC_CREAT);
    tMensaje msjMiel, msjAbejaMutex, msjTarro, msjComer;
    int abeja = *((int *) nroAbeja);
    while (1)
    {
        srand (time(NULL));
        msgrcv(colaAbejaMutex,&msjAbejaMutex,SIZE_MSG,TIPO_MSJ,0);
        msjMiel.type = TIPO_MSJ;
        msgsnd(colaMiel,&msjMiel,SIZE_MSG,IPC_NOWAIT);
        msgrcv(colaTarro,&msjTarro,SIZE_MSG,TIPO_MSJ,0);
        printf("La abeja %d agrega una porcion al tarro.\n",abeja + 1);
        sleep(1);
        if(msgrcv(colaTarro,&msjTarro,SIZE_MSG,TIPO_MSJ,IPC_NOWAIT) <= 0) {
            printf("La abeja %d despierta al oso.\n",abeja + 1);
            sleep(1);
            msjComer.type = TIPO_MSJ;
            msgsnd(colaComer,&msjComer,SIZE_MSG,IPC_NOWAIT);
        }
        else
        {
            msjTarro.type = TIPO_MSJ;
            msgsnd(colaTarro,&msjTarro,SIZE_MSG,IPC_NOWAIT);
            msgsnd(colaAbejaMutex,&msjAbejaMutex,SIZE_MSG,IPC_NOWAIT);
            sleep(rand() % CANT_ABEJAS + 1);
        }
        

    }

}

void* oso() {
    int colaMiel = msgget((key_t)COLA_MIEL, 0666 | IPC_CREAT);
    int colaTarro = msgget((key_t)COLA_TARRO, 0666 | IPC_CREAT);
    int colaComer = msgget((key_t)COLA_COMER, 0666 | IPC_CREAT);
    int colaAbejaMutex = msgget((key_t)COLA_ABEJA_MUTEX, 0666 | IPC_CREAT);
    tMensaje msjMiel, msjAbejaMutex, msjTarro, msjComer;
    int i;
    while (1)
    {
        msgrcv(colaComer,&msjComer,SIZE_MSG,TIPO_MSJ,0);
        printf("El oso es despertado por una abeja y come la miel.\n");
        for (i = 0; i < CANT_PORCIONES; i++)
        {
            msgrcv(colaMiel,&msjMiel,SIZE_MSG,TIPO_MSJ,0);
            printf("Una porción de miel consumida\n");
            sleep(1);
        }
        prepararTarro();
        sleep(1);
        printf("El oso se vuelve a dormir hasta tener otro tarro de miel\n");
        sleep(1);
        msjAbejaMutex.type = TIPO_MSJ;
        msgsnd(colaAbejaMutex,&msjAbejaMutex,SIZE_MSG,IPC_NOWAIT);
    }
    
    
}
