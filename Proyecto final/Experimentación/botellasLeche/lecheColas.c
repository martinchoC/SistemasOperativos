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

#define COLA_LECHE 1234
#define COLA_HELADERA 1235
#define COLA_SUPERMERCADO 1236 

#define CANT_LECHE 10

#define TIPO_SUPERMERCADO 1L
#define TIPO_HELADERA 2L
#define TIPO_LECHE 3L

#define CANT_COMPANEROS 5

void crearColas();
void llenarHeladera();
void habilitarHeladera();
void habilitarSupermercado();
void* mirarHeladera(void* nroCompanero);

void main() {
                     
    int i;

    crearColas();

    llenarHeladera();

    habilitarHeladera();
    printf("Heladera habilitada.\n");

    habilitarSupermercado();
    printf("Supermercado habilitado.\n");

    i = 0;  
    int comp;
    pthread_t hilosCompaneros[CANT_COMPANEROS]; //un hilo para cada compañero
    for(i=0; i < CANT_COMPANEROS; i++){
        comp = i + 1;
        pthread_create(&hilosCompaneros[i], NULL, mirarHeladera, &comp);
        sleep(1);
    }
    for(i=0; i<CANT_COMPANEROS;i++){
        pthread_join(hilosCompaneros[i], NULL);
    }

}

void crearColas() {
    int colaHeladera = msgget((key_t)COLA_HELADERA, 0666 | IPC_CREAT);
    if (colaHeladera < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
    
    int colaSupermercado = msgget((key_t)COLA_SUPERMERCADO, 0666 | IPC_CREAT);
    if (colaSupermercado < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaLeche = msgget((key_t)COLA_LECHE, 0666 | IPC_CREAT);
    if (colaLeche < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
}

void llenarHeladera() {
    int i = 1;
    tMensaje msjLeche;
    int colaLeche = msgget((key_t)COLA_LECHE, 0666 | IPC_CREAT);
    while (i <= CANT_LECHE)
    {
        msjLeche.type = TIPO_LECHE;
        msjLeche.body[0] = i + '0';
        printf("Leche %s agregada a la heladera.\n",msjLeche.body);
        msgsnd(colaLeche, &msjLeche, SIZE_MSG, IPC_NOWAIT);
        sleep(1);
        i++;
    }
}

void habilitarHeladera() {
    int colaHeladera = msgget((key_t)COLA_HELADERA, 0666 | IPC_CREAT);
    tMensaje msjHeladera;
    msjHeladera.type = TIPO_HELADERA;
    msjHeladera.body[0] = '1';
    msgsnd(colaHeladera,&msjHeladera,SIZE_MSG,IPC_NOWAIT);
    sleep(1);
}

void habilitarSupermercado() {
    int colaSupermercado = msgget((key_t)COLA_SUPERMERCADO, 0666 | IPC_CREAT);
    tMensaje msjSupermercado;
    msjSupermercado.type = TIPO_SUPERMERCADO;
    msjSupermercado.body[0] = '1';
    msgsnd(colaSupermercado,&msjSupermercado,SIZE_MSG,IPC_NOWAIT);
    sleep(1);
}

void* mirarHeladera(void* nCompanero)
{
    int nroCompanero = *((int*) nCompanero);
    int colaHeladera = msgget((key_t)COLA_HELADERA, 0666 | IPC_CREAT);
    int colaSupermercado = msgget((key_t)COLA_SUPERMERCADO, 0666 | IPC_CREAT);
    int colaLeche = msgget((key_t)COLA_LECHE, 0666 | IPC_CREAT);
    int nro;
    tMensaje msjHeladera, msjLeche, msjSupermercado, msjCompanero;
    while(1){
        if (msgrcv(colaHeladera,&msjHeladera,SIZE_MSG,TIPO_HELADERA,IPC_NOWAIT) > 0) {
            if(msgrcv(colaLeche,&msjLeche,SIZE_MSG,TIPO_LECHE,IPC_NOWAIT) > 0) {
                //puedo tomar leche
                printf("El compañero %d toma una leche de la heladera y la bebe\n", nroCompanero);
                habilitarHeladera();
                sleep(rand() % 2 + 1);
            }
            else if (msgrcv(colaSupermercado,&msjSupermercado,SIZE_MSG,TIPO_SUPERMERCADO,IPC_NOWAIT) > 0) //no hay leches, hay que comprar
            {
                sleep(1);
                printf("El compañero %d mira la heladera. No hay leche. Va a comprar\n",nroCompanero);
                sleep(1);
                habilitarHeladera();
                sleep(1);    
                printf("El compañero %d llega al supermercado\n",nroCompanero);
                sleep(1);
                printf("El compañero %d compra leche\n",nroCompanero);
                sleep(1);
                printf("El compañero %d llega a la casa; guarda la leche\n",nroCompanero);
                msgrcv(colaHeladera,&msjHeladera,SIZE_MSG,TIPO_HELADERA,0);
                for (int i = 0; i < CANT_LECHE; i++)
                {
                    msgsnd(colaLeche,&msjLeche,SIZE_MSG,IPC_NOWAIT);
                    nro = i+1;
                    printf("Leche %d repuesta\n",nro);
                    sleep(1);
                }
                habilitarSupermercado();
                habilitarHeladera();
            }
            else { //no se puede comprar ni hay leche
                printf("El compañero %d mira la heladera. No puede tomar porque no hay leche y no puede comprar porque hay alguien comprando. Vuelve más tarde\n", nroCompanero);
                habilitarHeladera();
                sleep(rand() % 3 +1);
            }
        }
        else { // heladera ocupada (exclusion, solo una persona a la vez en la heladera)
            printf("El compañero %d no puede ir a la heladera porque hay alguien. Vuelve más tarde\n", nroCompanero);
            sleep(rand() % 3 + 1);
        }        
    }
}