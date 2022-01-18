/**
 * ACLARACION:
 * El grafo se intepreto como una especie de grafo de productor consumidor, la flecha de Ti a Tj indica
 * que cuando  Ti termina Tj esta hablilitado para ejecutar. Varias flechas indican que cualquiera de los referenciados
 * pueden arrancar pero solo uno de ellos.
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>


// STRUCT QUE ALMACENA LA INFORMACION NECESARIA PARA CADA HILO.

struct tinfo{
	pthread_t id;														// IDENTIFICACION DEL HILO.
	int 	tnro;														// NRO DE IDENTIFICACION PROPIO.
	sem_t *semaforo;													// ARREGLO DE SEMAFOROS.
};


// CUERPO DE EJECUCION DEL HILO 1
void* hilo0(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		sleep(2);
		printf("Hilo 1 produjo un elemento.\n");
		sem_post((datos->semaforo));									// SIGNAL A SEMAFORO PARA AVISAR QUE TERMINO SU EJECUCION.
	}
}

// CUERPO DE EJECUCION DEL HILO 2
void* hilo1(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		int encontro=0;
		while(!encontro)
		{
			
			if (!sem_trywait(datos->semaforo))							// INTENTA VER SI TERMINO DE EJECUTAR 1.
			{	
				sleep(2);
				printf("Hilo 2 consumio de 1 y produjo un dato.\n");
				encontro=1;
			}
			else
			{
				if(!sem_trywait(datos->semaforo+3))						// INTENTA RESERVAR UN ELEMENTO DE 4
				{
					sleep(2);
					printf("Hilo 2 consumio de 4 y produjo un dato.\n");
					encontro=1;
				}
			}
		}
		sem_post((datos->semaforo+1));									// SIGNAL PARA AVISAR QUE YA TERMINO DE EJECUTAR 2. 
	}
	
}

// CUERPO DE EJECUCION DEL HILO 3
void* hilo2(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		sem_wait((datos->semaforo));									// WAIT ESPERANDO POR SIGNAL DE 1.
		sleep(2);
		printf("Hilo 3 consumio de 1 y produjo un dato.\n");
		sem_post((datos->semaforo+2));									// SIGNAL PARA AVISAR QUE TERMIO DE EJECUTAR.
	}
	
}

// CUERPO DE EJECUCION DEL HILO 4
void* hilo3(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		int encontro=0;
		while(!encontro)
		{
			if (!sem_trywait(datos->semaforo +1 ) )						// INTENTA RESERVAR DE HILO 2.
			{	
				sleep(2);
				printf("Hilo 4 consumio de 2 y produjo un dato.\n");
				encontro=1;
			}
			else
			{
				if(!sem_trywait(datos->semaforo +6))					// INTENTA RESERVAR DE HILO 7
				{
					sleep(2);
					printf("Hilo 4 consumio de 7 y produjo un dato.\n");
					encontro=1;
				}
			}
		}
		sem_post((datos->semaforo+3));									// SIGNAL PARA AVISAR QUE TERMINO DE EJECUTAR
	}
}

// CUERPO DE EJECUCION DEL HILO 5
void* hilo4(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		sem_wait((datos->semaforo));									// WAIT PARA ESPERAR POR DATO DE 1.
		sleep(2);
		printf("Hilo 5 consumio de 1 y produjo un dato.\n");
		sem_post((datos->semaforo+4));									//SIGNAL PARA AVISAR QUE TERMINO DE EJECUTAR.

	}
}

// CUERPO DE EJECUCION DEL HILO 6
void* hilo5(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		int encontro=0;
		while(!encontro)
		{
			if (!sem_trywait(datos->semaforo +2 ) )						// INTENTA RESERVAR DE HILO 3.
			{	
				sleep(2);
				printf("Hilo 6 consumio de 3 y produjo un dato.\n");
				encontro=1;
			}
			else
			{
				if(!sem_trywait(datos->semaforo +4))					// INTENTA RESERVAR DE HILO 5.
				{
					sleep(2);
					printf("Hilo 6 consumio de 5 y produjo un dato.\n");
					encontro=1;
				}
			}
		}
		sem_post((datos->semaforo+ 5));									// SIGNAL QUE TERMINO DE EJECUTAR.
	}
}

// CUERPO DE EJECUCION DEL HILO 7
void* hilo6(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		
		sem_wait((datos->semaforo+4));									// WAIT PARA ESPERAR POR HILO 5.
		sleep(2);
		printf("Hilo 7 consumio de 5 y produjo un dato.\n");
		sem_post((datos->semaforo+6));									// AVISA QUE TERMINO DE EJECUTAR.
	}
	sem_post((datos->semaforo+ 6));
}

// CUERPO DE EJECUCION DEL HILO 8
void* hilo7(void* dato)
{
	struct tinfo *datos = dato;											// RECUPERAMOS LOS DATOS RECIBIDOS POR PARAMETRO.
	while (1)
	{
		
		int encontro=0;
		while(!encontro)
		{
			if (!sem_trywait(datos->semaforo +5 ) )						// INTENTA RESERVAR DE 6.
			{	
				sleep(2);
				printf("Hilo 8 consumio de 6 y produjo un dato.\n");
				encontro=1;
			}
			else
			{
				if(!sem_trywait(datos->semaforo +3))					// INTENTA RESERVAR DE 4.
				{
					sleep(2);
					printf("Hilo 8 consumio de 4 y produjo un dato.\n");
					encontro=1;
				}
				else
				{
					if(!sem_trywait(datos->semaforo + 6))				// INTENTA RESERVAR DE 7.
					{
						sleep(2);
						printf("Hilo 8 consumio de 7 y produjo un dato.\n");
						encontro=1;
					}
				}
			}
		}
		sem_post((datos->semaforo+ 7));									// SIGNAL PARA AVISAR QUE TERMINO DE EJECUTAR.
	}
}


int main()
{
	struct tinfo T[8];													// ARREGLO DE 8 HILOS.
	sem_t S[7];															// ARREGLO DE 7 SEMAFOROS
	
	// INICIALIZAMOS LOS SEMAFOROS
	sem_init(&S[0], 0,0);												
	sem_init(&S[1], 0,0);												
	sem_init(&S[2], 0,0);
	sem_init(&S[3], 0,0);
	sem_init(&S[4], 0,0);
	sem_init(&S[5], 0,0);
	sem_init(&S[6], 0,0);
	
	// CREAMOS LOS 8 HILOS Y LE PASAMOS LOS PARAMETROS NECESARIOS.
	int i=0;
	for(i;i<8;i++)
	{
		T[i].tnro=i;
		T[i].semaforo = S;
		crearHilo(i, &T[i]);
	}
		
	// ESPERAMOS QUE TERMINEN LOS 8 HILOS.
	i=0;
	for(i;i<8;i++)
		pthread_join(T[i].id, NULL);
	
	
	return 0;
	
}

// METODO QUE DE ACUERDO AL ENTERO PASADO POR PARAMETRO CREA CADA HILO CON SU CUERPO DE EJECUCION PARTICULAR
int crearHilo(int i, struct tinfo *T)
{
	switch (i)
		{
			case 0:
			{
				pthread_create(&(T->id), NULL, hilo0, (T)); break;
			}
			case 1:
			{
				pthread_create(&(T->id), NULL, hilo1, (T)); break;
			}
			case 2:
			{
				pthread_create(&(T->id), NULL, hilo2, (T)); break;
			}
			case 3:
			{
				pthread_create(&(T->id), NULL, hilo3, (T)); break;
			}
			case 4:
			{
				pthread_create(&(T->id), NULL, hilo4, (T)); break;
			}
			case 5:
			{
				pthread_create(&(T->id), NULL, hilo5, (T)); break;
			}
			case 6:
			{
				pthread_create(&(T->id), NULL, hilo6, (T)); break;
			}
			case 7:
			{
				pthread_create(&(T->id), NULL, hilo7, (T)); break;
			}
		}
			
}

