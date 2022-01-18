#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h> 

/*Estructuras de tareas*/
struct persona
{
    char nombrePersona;
};

sem_t leche, supermercado, heladera; //heladera es un mutex para resguardar cuando se estén guardando las leches

void ejecutarCompaneros();
void* mirarHeladera(void* companeroEncargado);

int main() {
    srand(time(NULL));
    ejecutarCompaneros();
    return 0;
}

void ejecutarCompaneros() {
    int i; //para ciclar en la creacion de hilos
    pthread_t threads[2]; //un hilo para cada compañero

	struct persona thread_companeros[2]={{'A'}, {'B'}};

    //inicialmente habrá 10 leches en la heladera
    sem_init(&leche, 0, 10);

    //inicialmente no se necesita comprar leche
	sem_init(&supermercado, 0, 1);

    sem_init(&heladera,0,1);
		
	for(i=0; i<2; i++)
		pthread_create(&threads[i], NULL, mirarHeladera, &thread_companeros[i]);
	
	for(i=0; i<2; i++)
		pthread_join(threads[i], NULL);
		
}

void* mirarHeladera(void* companeroEncargado)
{
    while(1){
        int nro;
        struct persona *companero=(struct persona*)companeroEncargado;
        if (sem_trywait(&heladera) == 0) {
            if(sem_trywait(&leche) == 0) {
                //puedo tomar leche
                printf("El compañero %c toma una leche de la heladera y la bebe\n", companero->nombrePersona);
                sem_post(&heladera);
                sleep(rand() % 2 + 1);
            }
            else if (sem_trywait(&supermercado) == 0) //no hay leches, hay que comprar
            {
                sleep(1);
                printf("El compañero %c mira la heladera. No hay leche. Va a comprar\n",companero->nombrePersona);
                sleep(1);
                sem_post(&heladera);
                sleep(1);    
                printf("El compañero %c llega al supermercado\n",companero->nombrePersona);
                sleep(1);
                printf("El compañero %c compra leche\n",companero->nombrePersona);
                sleep(1);
                printf("El compañero %c llega a la casa; guarda la leche\n",companero->nombrePersona);
                sem_wait(&heladera);
                for (int i = 0; i < 10; i++)
                {
                    sem_post(&leche);
                    nro = i+1;
                    printf("Leche %d repuesta\n",nro);
                    sleep(1);
                }
                sem_post(&supermercado); 
                sem_post(&heladera);
            }
            else { //no se puede comprar ni hay leche
                printf("El compañero %c mira la heladera. No puede tomar porque no hay leche y no puede comprar porque hay alguien comprando. Vuelve más tarde\n", companero->nombrePersona);
                sem_post(&heladera);
                sleep(rand() % 3 +1);
            }
        }
        else { // heladera ocupada (exclusion, solo una persona a la vez en la heladera)
            printf("El compañero %c no puede ir a la heladera porque hay alguien. Vuelve más tarde\n", companero->nombrePersona);
            sleep(rand() % 3 + 1);
        }        
    }
    pthread_exit(0);
}