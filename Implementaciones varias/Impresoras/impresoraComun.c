#include <unistd.h>         
#include <stdio.h>          
#include <stdlib.h>         
#include <pthread.h>        
#include <semaphore.h>  
#include <time.h>

#define CANT_USUARIOS 20        
#define CANT_IMPRESORAS 2      
 
sem_t semUsuario;               
sem_t semImpresora;           

void* imprimir(void *tmp); 
          
int main()
{   
    pthread_t hiloImpresora[CANT_IMPRESORAS], hiloUsuario[CANT_USUARIOS]; 
    int i,nroUsuario;
    int usuario[CANT_USUARIOS];

    sem_init(&semImpresora,0,CANT_IMPRESORAS);

    printf("Oficina de impresoras abierta!!\n");

    for(i = 0; i < CANT_USUARIOS; i++)                     
    {   
        nroUsuario = i + 1;
        pthread_create(&hiloUsuario[i], NULL, imprimir, &nroUsuario);
        usleep(100);
    }   
    
    for(i=0;i<CANT_USUARIOS;i++)       
        pthread_join(hiloUsuario[i],NULL);

    exit(EXIT_SUCCESS); 
}
 
void* imprimir(void *tmp)  
{
    int usr = *((int*) tmp);
    while (1)
    {
        srand(time(NULL));
        if(sem_trywait(&semImpresora) == 0){
            printf("El usuario %d usa una impresora.\n",usr);
            sleep(rand() % 3 + 1);
            printf("El usuario %d terminó de imprimir y libera una impresora.\n",usr);
            sem_post(&semImpresora);
        }
        sleep(rand() % 2 + 1);
    }
    pthread_exit(0);
}
 