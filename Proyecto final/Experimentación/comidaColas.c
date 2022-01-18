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
//gcc comidaColas.c -o comidaColas -lrt -pthread

//PARA VER ESTADO DE COLAS
//ipcs -q

//PARA BORRAR COLAS
//ipcrm --all=msg

struct message {
  long type;
  char body[20];
};
typedef struct message tMensaje;

#define SIZE_MSG sizeof(tMensaje) - sizeof(long)

#define CANT_MESAS_LIMPIAS 30
#define CANT_MESAS_SUCIAS 0 
#define CANT_COCINEROS 3
#define CANT_CAMAREROS 1
#define CANT_LIMPIADORES 1
#define CANT_CLIENTES 50
#define CAPACIDAD_COLA 10

#define COLA_MESAS 1234
#define COLA_CAMARERO 1235 
#define COLA_PEDIDOS 1236
#define COLA_DISPONIBLE 1237 
#define COLA_COMIDA_LISTA 1238
#define COLA_COMER 1239

#define TIPO_CARNE 1L
#define TIPO_VEGE 2L
#define TIPO_MESA_LIMPIA 3L
#define TIPO_MESA_SUCIA 4L

void crearColas();
void inicializarColas();

void* cliente(void* nombre);
void* camarero();
void* limpiador();
void* cocinero(void* nombre);

void main() {
    
    //creo las colas
    crearColas();

    //inicialización de colas
    inicializarColas();

    int i; //para ciclar en la creación de hilos
    pthread_t hilosCocineros[CANT_COCINEROS]; //un hilo para cada cocinero
    pthread_t hilosCamareros[CANT_CAMAREROS]; //un hilo para cada camarero
    pthread_t hilosLimpiadores[CANT_LIMPIADORES]; //un hilo para cada limpiador
    pthread_t hilosclientes[CANT_CLIENTES]; //un hilo para cada cliente

    //creación de los hilos para cada    
    for(i = 0; i < CANT_CAMAREROS; i++) 
    {
        pthread_create(&hilosCamareros[i], NULL, camarero, NULL);
    }

    for (i = 0; i < CANT_COCINEROS; i++)
    {
        pthread_create(&hilosCocineros[i], NULL, cocinero, &i);
        sleep(1);
    }

    for(i = 0; i < CANT_LIMPIADORES; i++) 
    {
        pthread_create(&hilosLimpiadores[i], NULL, limpiador, NULL);
    }

    for (i = 0; i < CANT_CLIENTES; i++)
    {
        pthread_create(&hilosclientes[i], NULL, cliente, &i);
        sleep(rand() % 2 + 1);
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

void crearColas() {
    int colaMesas = msgget((key_t)COLA_MESAS, 0666 | IPC_CREAT);
    if (colaMesas < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }
    
    int colaCamarero = msgget((key_t)COLA_CAMARERO, 0666 | IPC_CREAT);
    if (colaCamarero < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaPedidos = msgget((key_t)COLA_PEDIDOS, 0666 | IPC_CREAT);
    if (colaPedidos < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaDisponible = msgget((key_t)COLA_DISPONIBLE, 0666 | IPC_CREAT);
    if (colaDisponible < 0)
    {
        perror("cola");
        exit(EXIT_FAILURE);
    }

    int colaComidaLista = msgget((key_t)COLA_COMIDA_LISTA, 0666 | IPC_CREAT);
    if (colaComidaLista < 0)
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
}

void inicializarColas() {
    int i;
    
    i = 1;
    //10 lugares como máximo en la cola de comida lista
    tMensaje msjDisponible;
    int colaDisponible = msgget((key_t)COLA_DISPONIBLE, 0666 | IPC_CREAT);
    while (i <= CAPACIDAD_COLA)
    {
        msjDisponible.type = 1L;
        printf("Nuevo lugar disponible para cocinar.\n");
        msgsnd(colaDisponible, &msjDisponible, SIZE_MSG, IPC_NOWAIT);
        sleep(1);
        i++;
    }

    i = 1;
    //30 lugares como máximo en la cola de sillas disponibles
    tMensaje msjMesa;
    int colaMesas = msgget((key_t)COLA_MESAS, 0666 | IPC_CREAT);
    while (i <= CANT_MESAS_LIMPIAS)
    {
        msjMesa.type = TIPO_MESA_LIMPIA;
        printf("Mesa %d agregada a la cola de mesas disponibles.\n",i);
        msgsnd(colaMesas, &msjMesa, SIZE_MSG, IPC_NOWAIT);
        sleep(1);
        i++;
    }

}

void* cliente(void* nombre) {
    int nroCliente = *((int *) nombre);
    int carneVege;
    int colaMesas = msgget((key_t)COLA_MESAS, 0666 | IPC_CREAT);
    int colaCamarero = msgget((key_t)COLA_CAMARERO, 0666 | IPC_CREAT);
    int colaComidaLista = msgget((key_t)COLA_COMIDA_LISTA, 0666 | IPC_CREAT);
    int colaComer = msgget((key_t)COLA_COMER, 0666 | IPC_CREAT);
    tMensaje msjMesa, msjCamarero, msjComidaLista;
    while(1) {
        srand (time(NULL));
        msgrcv(colaMesas, &msjMesa,SIZE_MSG,TIPO_MESA_LIMPIA,0);
        printf("El cliente %d se sienta en una mesa limpia.\n", nroCliente+1);
        sleep(1);
        carneVege = rand() % 2;
        if (carneVege == 0)
        {
            msjCamarero.type = TIPO_CARNE;
            printf("El cliente %d pide comida con carne.\n", nroCliente+1);

        }
        else
        {
            msjCamarero.type = TIPO_VEGE;
            printf("El cliente %d pide comida vegetariana.\n", nroCliente+1);
        }        
        sleep(1);
        msgsnd(colaCamarero, &msjCamarero, SIZE_MSG, IPC_NOWAIT);
        
        msgrcv(colaComer, &msjComidaLista, SIZE_MSG, msjCamarero.type, 0);
        if (msjComidaLista.type == TIPO_CARNE)
        {
            printf("Comida con carne servida al cliente %d.\n",nroCliente+1);
        }
        else if (msjComidaLista.type == TIPO_VEGE)
        {
            printf("Comida vegetariana servida al cliente %d.\n",nroCliente+1);
        }
        sleep(1);
        printf("El cliente %d está comiendo.\n", nroCliente+1);
        sleep(1);
        printf("El cliente %d se retira del restaurant.\n", nroCliente+1);
        msjMesa.type = TIPO_MESA_SUCIA;
        msgsnd(colaMesas, &msjMesa, SIZE_MSG, IPC_NOWAIT);
        pthread_exit(0);
    }
}

void* camarero() {
    int colaCamarero = msgget((key_t)COLA_CAMARERO, 0666 | IPC_CREAT);
    int colaComidaLista = msgget((key_t)COLA_COMIDA_LISTA, 0666 | IPC_CREAT);
    int colaDisponible = msgget((key_t)COLA_DISPONIBLE, 0666 | IPC_CREAT);
    int colaPedidos = msgget((key_t)COLA_PEDIDOS, 0666 | IPC_CREAT);
    int colaComer = msgget((key_t)COLA_COMER, 0666 | IPC_CREAT);
    tMensaje msjCamarero, msjComida, msjDisponible;
    while (1)
    {
        msgrcv(colaCamarero, &msjCamarero, SIZE_MSG, 0, 0);
        
        if (msjCamarero.type == TIPO_CARNE)
        {
            msgsnd(colaPedidos, &msjCamarero, SIZE_MSG, IPC_NOWAIT);
            sleep(1);
            msgrcv(colaComidaLista, &msjComida, SIZE_MSG, TIPO_CARNE, 0);
            printf("El camarero busca una comida con carne\n");
        }
        else
        {
            msgsnd(colaPedidos, &msjCamarero, SIZE_MSG, IPC_NOWAIT);
            sleep(1);
            msgrcv(colaComidaLista, &msjComida, SIZE_MSG, TIPO_VEGE, 0);
            printf("El camarero busca una comida vegetariana\n");
        }
        printf("Lugar libre para cocinar.\n");
        sleep(1);
        msjDisponible.type = 1L;
        msgsnd(colaDisponible, &msjDisponible, SIZE_MSG, IPC_NOWAIT);
        if (msjComida.type == TIPO_CARNE)
        {
            printf("El camarero lleva una comida con carne\n");
        }
        else if (msjComida.type == TIPO_VEGE)
        {
            printf("El camarero lleva una comida vegetariana\n");
        }
        msgsnd(colaComer, &msjComida, SIZE_MSG, IPC_NOWAIT);
        sleep(1);
    }
    pthread_exit(0);
}

void* limpiador() {
    int colaMesas = msgget((key_t)COLA_MESAS, 0666 | IPC_CREAT);
    tMensaje msjMesa;
    while (1)
    {
        msgrcv(colaMesas,&msjMesa,SIZE_MSG,TIPO_MESA_SUCIA,0);
        sleep(rand() % 3 + 1);
        printf("El limpiador está limpiando una mesa sucia.\n");
        sleep(rand() % 3 + 1);
        printf("Hay una mesa limpia para ser usada.\n");
        sleep(1);
        msjMesa.type = TIPO_MESA_LIMPIA;
        msgsnd(colaMesas,&msjMesa,SIZE_MSG,IPC_NOWAIT);
    }
    pthread_exit(0);
}

void* cocinero(void* nombre) {
    int nroCocinero = *((int *) nombre);
    int colaComidaLista = msgget((key_t)COLA_COMIDA_LISTA, 0666 | IPC_CREAT);
    int colaDisponible = msgget((key_t)COLA_DISPONIBLE, 0666 | IPC_CREAT);
    int colaPedidos = msgget((key_t)COLA_PEDIDOS, 0666 | IPC_CREAT);
    tMensaje msjDisponible, msjComida;
    while (1)
    {
        msgrcv(colaDisponible, &msjDisponible, SIZE_MSG, 0, 0); //si hay lugar para cocinar en la fila, lo consumo... "NO PUEDO TENER MAS DE 10 PLATOS LISTOS EN LA FILA!!!"
        msgrcv(colaPedidos, &msjComida, SIZE_MSG, 0, 0); //consumo un pedido para cocinar
        if (msjComida.type == TIPO_CARNE)
        {
            printf("El cocinero %d va a cocinar una porción de comida con carne.\n",nroCocinero+1);
        }
        else if (msjComida.type == TIPO_VEGE)
        {
            printf("El cocinero %d va a cocinar una porción de comida vegetariana.\n",nroCocinero+1);
        }
        sleep(1);
        printf("El cocinero %d está cocinando...\n",nroCocinero+1);
        sleep(1);
        if (msjComida.type == TIPO_CARNE)
        {
            printf("El cocinero %d terminó una comida con carne y la agregó a la cola.\n",nroCocinero+1);
        }
        else if (msjComida.type == TIPO_VEGE)
        {
            printf("El cocinero %d terminó una comida vegetariana y la agregó a la cola.\n",nroCocinero+1);
        }
        msgsnd(colaComidaLista, &msjComida, SIZE_MSG, IPC_NOWAIT);
        sleep(1);
    }
    pthread_exit(0);
}