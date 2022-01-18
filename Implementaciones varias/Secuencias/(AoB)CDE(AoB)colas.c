#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>

int qABAB; //Cola entre (A o B) y (A o B)
int qABC; //Cola entre (A o B) y C
int qCD; //Cola entre C y D
int qDE; //Cola entre D y E
int qEAB; //Cola entre E y (A o B)

void procesoA();
void procesoB();
void procesoC();
void procesoD();
void procesoE();
void crearcolas();

typedef struct message
{
  	long mtype;
	int msg; 
} m;

m enviado;
m recibido;

/* 	Secuencia (AoB)CDE(AoB)... con cola de mensajes.
	Colas: (AoB)<->(AoB); (AoB)->C; C->D; D->E; E->(AoB) */
int main(int argc, char *argv[])
{    	
	pid_t pidB;
	pid_t pidC;
	pid_t pidD;
	pid_t pidE;
	
	crearcolas();
	
	enviado.mtype = 1;
	enviado.msg = 1;

    pidB = fork();
    if (pidB == -1) {
        perror("fork B");
        exit(EXIT_FAILURE);
    }
    if (pidB == 0) {  /* Proceso B  */
	/* Creo el proceso c */
	pidC = fork();
	if (pidC == -1) { //Error fork C
		perror("fork C");
		exit(EXIT_FAILURE);
    	}

    	if (pidC == 0) { /* Proceso C  */
		/* Creo el proceso E */
		pidE = fork();
		if (pidE == -1) { //Error fork E
			perror("fork E");
			exit(EXIT_FAILURE);
	    	}

	    	if (pidE == 0) { /* Proceso E  */
			/* Creo el proceso D */
			pidD = fork();
			if (pidD == -1) { //Error fork D
				perror("fork D");
				exit(EXIT_FAILURE);
		    	}

		    	if (pidD == 0) { /* Proceso D  */
				procesoD();
				_exit(EXIT_SUCCESS);
			}	
			else { /* Proceso E  */
				procesoE();
				_exit(EXIT_SUCCESS);
			}	
		}	
		else { /* Proceso C  */
			procesoC();
			_exit(EXIT_SUCCESS);
		}		
	}	
	else { /* Proceso B  */
		procesoB();
		_exit(EXIT_SUCCESS);
	}
    } else {  /* Proceso A */
	//Enviar el mensaje a (A o B)
	if((msgsnd(qABAB , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1)
	{	
		printf("Error al enviar mensaje inicial a A o B\n");
		_exit(1);
	}
      	procesoA();
        exit(EXIT_SUCCESS);
    }
}

void procesoA(){
	while(1){
		while(msgrcv(qABAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); //Leo de (A o B)
		printf("A\n");
		if(recibido.msg == 1){ //es la primera A, escribo a C, espero a E
			if((msgsnd(qABC , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1) //Escribo a C
			{
				printf("A: Error al enviar mensaje a C\n");
				_exit(1);
			}
			while(msgrcv(qEAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 	 //Espero a E
			enviado.msg = 2; //indico que tiene que imprimirse la segunda (A o B)
		}else if(recibido.msg == 2){ //La segunda (A o B) ya se imprimio, tiene que imprimir la tercera
			enviado.msg = 3; //indico que tiene que imprimirse la tercera (A o B)
			}
			else{ //La tercera (A o B) ya se imprimio, vuelvo a escribir a C, espero a E, y vuelvo a empezar
				if((msgsnd(qABC , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1) //Escribo a C
				{
					printf("A: Error al enviar mensaje a C\n");
					_exit(1);
				}
				while(msgrcv(qEAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 	 //Espero a E
				enviado.msg = 1; //indico que tiene que imprimirse la segunda (A o B)
				printf("\n");
				sleep(1);
			}
		if((msgsnd(qABAB, &enviado, sizeof(enviado) - sizeof(long), 0)) == -1)	//Escribo a (A o B)
			{
				printf("A: Error al enviar mensaje a A o B\n");
				_exit(1);
			}
	}
}

void procesoB(){
	while(1){
		while(msgrcv(qABAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); //Leo de (A o B)
		printf("B\n");
		if(recibido.msg == 1){ //es la primera B, escribo a C, espero a E
			if((msgsnd(qABC , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1) //Escribo a C
			{
				printf("B: Error al enviar mensaje a C\n");
				_exit(1);
			}
			while(msgrcv(qEAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 	 //Espero a E
			enviado.msg = 2; //indico que tiene que imprimirse la segunda (A o B)
		}else if(recibido.msg == 2){ //La segunda (A o B) ya se imprimio, tiene que imprimir la tercera
			enviado.msg = 3; //indico que tiene que imprimirse la tercera (A o B)
			}
			else{ //La tercera (A o B) ya se imprimio, vuelvo a escribir a C, espero a E, y vuelvo a empezar
				if((msgsnd(qABC , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1) //Escribo a C
				{
					printf("A: Error al enviar mensaje a C\n");
					_exit(1);
				}
				while(msgrcv(qEAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 	 //Espero a E
				enviado.msg = 1; //indico que tiene que imprimirse la segunda (A o B)
				printf("\n");
				sleep(1);
			}
		if((msgsnd(qABAB, &enviado, sizeof(enviado) - sizeof(long), 0)) == -1) 	//Escribo a (A o B)
			{
				printf("B: Error al enviar mensaje a A o B\n");
				_exit(1);
			}
	}
}

void procesoC(){
	while(1){
		//Espero a (A o B)
		while(msgrcv(qABC, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 
		
		printf("C\n");

		//Enviar el mensaje a D
		if((msgsnd(qCD , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1)
		{	
			printf("C: Error al enviar mensaje a D\n");
			_exit(1);
		}
	}
}

void procesoD(){
	while(1){
		//Espero a C
		while(msgrcv(qCD, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 
	
		printf("D\n");
		
		//Enviar el mensaje a E
		if((msgsnd(qDE , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1)
		{	
			printf("D: Error al enviar mensaje a E\n");
			_exit(1);
		}
	}
}
   	
void procesoE(){
	while(1){
		//Espero a D
		while(msgrcv(qDE, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) == -1); 
	
		printf("E\n");

		//Enviar el mensaje a (A o B)
		if((msgsnd(qEAB , &enviado, sizeof(enviado) - sizeof(long), 0)) == -1)
		{	
			printf("E: Error al enviar mensaje a A o B\n");
			_exit(1);
		}
	}
}

void crearcolas(){
	/* Generar la key IPC ABC */
        key_t msgkeyabab = ftok(".", 'b');
	
	/* Crear la cola AB->C */
	if((qABAB = msgget( msgkeyabab, IPC_CREAT | 0660 )) == -1)
        {
		printf("Error al crear la cola de mensajes ABAB\n");
               _exit(1);
        }

	/* Si la cola ya existia, borro los mensajes viejos de la cola */
	while(msgrcv(qABAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) != -1);

   	/* Generar la key IPC ABC */
        key_t msgkeyabc = ftok(".", 'a');
	
	/* Crear la cola AB->C */
	if((qABC = msgget( msgkeyabc, IPC_CREAT | 0660 )) == -1)
        {
		printf("Error al crear la cola de mensajes ABC\n");
                _exit(1);
        }
	/* Si la cola ya existia, borro los mensajes viejos de la cola */
	while(msgrcv(qABC, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) != -1);

	/* Generar la key IPC C->D */
        key_t msgkeycd = ftok(".", 'c');

	/* Crear la cola C->D */
	if((qCD = msgget( msgkeycd, IPC_CREAT | 0660 )) == -1)
        {
		printf("Error al crear la cola de mensajes CD\n");
                _exit(1);
        }
	/* Si la cola ya existia, borro los mensajes viejos de la cola */
	while(msgrcv(qCD, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) != -1);

	/* Generar la key IPC D->E */
        key_t msgkeyde = ftok(".", 'd');

	/* Crear la cola D->E*/
	if((qDE = msgget( msgkeyde, IPC_CREAT | 0660 )) == -1)
        {
		printf("Error al crear la cola de mensajes DE\n");
                _exit(1);
        }
	/* Si la cola ya existia, borro los mensajes viejos de la cola */
	while(msgrcv(qDE, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) != -1);

	/* Generar la key IPC EAB */
        key_t msgkeyeab = ftok(".", 'e');

	/* Crear la cola AB->C */
	if((qEAB = msgget( msgkeyeab, IPC_CREAT | 0660 )) == -1)
        {
		printf("Error al crear la cola de mensajes EAB\n");
                _exit(1);
        }
	/* Si la cola ya existia, borro los mensajes viejos de la cola */
	while(msgrcv(qEAB, &recibido, sizeof(recibido) - sizeof(long),1, IPC_NOWAIT) != -1);
}
