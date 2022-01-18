#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> 
#include <sys/wait.h>

//métodos que se usarán
int generarCantidadLlantas();
char generarLetraCodigoColor();
void procesoTareaA();
void procesoTareaB();
void procesoTareaC();
void* realizarTareaA(void* tA);
void* realizarTareaB(void* tB);
void* realizarTareaC(void* tC);
char* concat(char *s1, char *s2);
char* concatChar(char *s1, char s2);

//declaro un buffer para almacenar el mensaje
/*Estructura de tareas que enviará el proceso coordinador*/
struct tarea
{
    int cantTareas;
    char color;
	char tipo;
    int cantRuedas;
};
typedef struct tarea tTarea;
#define SIZE_MSG_TAREA sizeof(tTarea)

/*Estructura del mensaje que se informará al proceso coordinador*/
struct message
{
    char msj[100];
};
typedef struct message tMensaje;
#define SIZE_MSG_COORDINADOR sizeof(tMensaje)

int pipeA[2];
int pipeB[2];
int pipeC[2];
int pipeRetorno[2];

sem_t ciclado;

int main() {
   
    //inicializo semáforo de exclusión mutua para usar en las tareas
    sem_init(&ciclado, 0, 1); 
    
    //inicializo los pipes
    if (pipe(pipeA) == -1) {
        perror("Pipe");
        exit(EXIT_FAILURE);
    }    
    if (pipe(pipeB) == -1) {
        perror("Pipe");
        exit(EXIT_FAILURE);
    }    
    if (pipe(pipeC) == -1) {
        perror("Pipe");
        exit(EXIT_FAILURE);
    }    
    if (pipe(pipeRetorno) == -1) {
        perror("Pipe");
        exit(EXIT_FAILURE);
    }    

    //declaro la tarea a realizar y el mensaje a recibir
    tTarea tarea;   
    tMensaje mensaje;

    //declaro los procesos hijos
    pid_t pidA, pidB, pidC;

    int n;

    //creo un numero random de ciclos
    srand(time(NULL));
    int randomCiclos = (rand() % (6 + 1 - 4)) + 4;
    printf("\n**********COMIENZA UN CICLO de %d TAREAS**********\n", randomCiclos);
    
    //creo conjunto de tareas
    tarea.cantTareas = randomCiclos;
    srand(time(NULL));
    tarea.color = generarLetraCodigoColor();
    tarea.tipo = rand() % 2 + '0';
    tarea.cantRuedas = generarCantidadLlantas();

    fflush(stdout);
    pidA = fork();
    if (pidA < 0)
    {
        perror("Proceso A");
        exit(EXIT_FAILURE);
    }
    if (pidA == 0) //hijo
    {
        /* escribo en el pipe A */
        write(pipeA[1],&tarea, SIZE_MSG_TAREA);
        procesoTareaA();
    }
    else //padre
    {
        wait(NULL);
        if (close(pipeRetorno[1]) != 1)
        {
            n=2;
            while (n>0)
            {
                read(pipeRetorno[0],&mensaje, SIZE_MSG_COORDINADOR);
                sleep(1);
                printf("%s",mensaje.msj);
                n--;
            }
        }
        else{
            printf("error al cerrar el pipe para lectura");
            exit(EXIT_FAILURE);
        } 
    }

    fflush(stdout);
    pidB = fork();
    if (pidB < 0)
    {
        perror("Proceso B");
        exit(EXIT_FAILURE);
    }
    if (pidB == 0) //hijo
    {
        /* escribo en el pipe B */
        write(pipeB[1],&tarea, SIZE_MSG_TAREA);
        procesoTareaB();
    }
    else //padre
    {
        wait(NULL);
        if (close(pipeRetorno[1]) != 1)
        {   
            n = 2;
            if(randomCiclos == 5) {
                read(pipeRetorno[0],&mensaje, SIZE_MSG_COORDINADOR);
                sleep(1);
                printf("%s",mensaje.msj);
                n--;
            }
            else
            {
                while (n>0)
                    {
                        read(pipeRetorno[0],&mensaje, SIZE_MSG_COORDINADOR);
                        sleep(1);
                        printf("%s",mensaje.msj);
                        n--;
                    }
            }
            
            
        }
        else{
            printf("error al cerrar el pipe para lectura");
            exit(EXIT_FAILURE);
        } 
    }


    fflush(stdout);
    pidC = fork();
    if (pidC < 0)
    {
        perror("Proceso A");
        exit(EXIT_FAILURE);
    }
    if (pidC == 0) //hijo
    {
        /* escribo en el pipe C */
        write(pipeC[1],&tarea, SIZE_MSG_TAREA);
        procesoTareaC();
    }
    else //padre
    {
        wait(NULL);
        if (close(pipeRetorno[1]) != 1)
        {
            n=2;
            if (randomCiclos == 4)
            {
                read(pipeRetorno[0],&mensaje, SIZE_MSG_COORDINADOR);
                printf("%s",mensaje.msj);
            }
            else {
                while (n>0)
                {
                    read(pipeRetorno[0],&mensaje, SIZE_MSG_COORDINADOR);
                    sleep(1);
                    printf("%s",mensaje.msj);
                    n--;
                }
            }
            
        }
        else{
            printf("error al cerrar el pipe para lectura");
            exit(EXIT_FAILURE);
        }
    }
    exit(0);
    
    
}

//genero un numero random para la cantidad de llantas de un vehículo 
//(asumo que pueden ser hasta 10, teniendo en cuenta solo los valores del arreglo cantidadLlantas)
int generarCantidadLlantas(){
    int cantidadLlantas[4]={1,2,3,4};
    int numRandom = rand() % 4;
    return cantidadLlantas[numRandom];
}

//genero un color random, definido por un caracter únicamente (Colores A, B, C, ..., Z)
char generarLetraCodigoColor() {
    char randomLetter = 'A' + (rand() % 26);
    return randomLetter;
}

//proceso tarea A
void procesoTareaA()
{
    //cierro el descriptor de escritura, va a leer del pipe
    close(pipeA[1]);

    tTarea tarea;
    read(pipeA[0],&tarea, SIZE_MSG_TAREA);
    int i; //para ciclar en la creacion de hilos
    pthread_t threads[2]; //un hilo para cada tipo de tarea

    for(i=0; i<2; i++){
		pthread_create(&threads[i], NULL, realizarTareaA, &tarea);
    }

    for(i=0; i<2; i++){
		pthread_join(threads[i], NULL);
    }

}

void procesoTareaB()
{
    //cierro el descriptor de escritura, va a leer del pipe
    close(pipeB[1]);

    tTarea tarea;
    read(pipeB[0],&tarea, SIZE_MSG_TAREA);
    int i; //para ciclar en la creacion de hilos
    int cant = 2;
    pthread_t threads[2]; //un hilo para cada tipo de tarea como maximo

    if (tarea.cantTareas == 5) {
        pthread_create(&threads[0], NULL, realizarTareaB, &tarea);
        pthread_join(threads[0], NULL);
    }
    else
    {
        for(i=0; i<cant; i++)
            pthread_create(&threads[i], NULL, realizarTareaB, &tarea);

        for(i=0; i<cant; i++)
            pthread_join(threads[i], NULL);
    }
}

void procesoTareaC()
{
    //cierro el descriptor de escritura, va a leer del pipe
    close(pipeC[1]);

    tTarea tarea;
    tMensaje mensaje;
    read(pipeC[0],&tarea, SIZE_MSG_TAREA);
    int i; //para ciclar en la creacion de hilos
    int cant = 2;
    pthread_t threads[2]; //un hilo para cada tipo de tarea

    if (tarea.cantTareas > 4)
    {
        for(i=0; i<cant; i++)
		    pthread_create(&threads[i], NULL, realizarTareaC, &tarea);

        for(i=0; i<cant; i++)
            pthread_join(threads[i], NULL);
    }
    else
    {
        char *mens = "";
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
        write(pipeRetorno[1],&mensaje, SIZE_MSG_COORDINADOR);
    }
      
}

void* realizarTareaA(void* tA)
{
    sem_wait(&ciclado);
	struct tarea *task=(struct tarea*)tA;
    tMensaje mensaje;
    char *mens;
    char *col = &task->color;
    int tiempo;
    if ( (task->tipo) == '0')
    {
        mens = concatChar("Trabajo de pintura de tipo parcial con 1 unidad de tiempo utilizada y de color ", task->color);
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
    }
    else if ( (task->tipo) == '1')
    {
        mens = concatChar("Trabajo de pintura de tipo total con 3 unidades de tiempo utilizadas y de color ", task->color);
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
    }
    write(pipeRetorno[1],&mensaje, SIZE_MSG_COORDINADOR);
    sem_post(&ciclado);
}

void* realizarTareaB(void* tB)
{
    sem_wait(&ciclado);
	struct tarea *task=(struct tarea*)tB;
    tMensaje mensaje;
    char *mens;
    char *col = &task->color;
    int tiempo;
    if ( (task->tipo) == '0')
    {
        mens = "Trabajo de frenos de tipo verificación con un tiempo 1\n";
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
    }
    else if ( (task->tipo) == '1')
    {
        mens = "Trabajo de frenos de tipo reparación con un tiempo 3\n";
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
    }
    write(pipeRetorno[1],&mensaje, SIZE_MSG_COORDINADOR);
    sem_post(&ciclado);
}

void* realizarTareaC(void* tC)
{
    sem_wait(&ciclado);
	struct tarea *task=(struct tarea*)tC;
    tMensaje mensaje;
    char *mens;
    int ruedas = task->cantRuedas;
    char llantas[5];
    snprintf(llantas, sizeof(llantas), "%d", ruedas);
    int tiempo;
    if ( (task->tipo) == '0')
    {
        mens = concat("Trabajo de reparacion de llantas. Unidades de tiempo utilizadas (1 unidad/llanta): ", llantas);
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
    }
    else if ( (task->tipo) == '1')
    {
        mens = "Trabajo de rotacion y balanceo con 3 unidades de tiempo utilizadas\n";
        memcpy(mensaje.msj, mens, SIZE_MSG_COORDINADOR);
    }
    write(pipeRetorno[1],&mensaje, SIZE_MSG_COORDINADOR);
    sem_post(&ciclado);
}

//metodo auxiliar para concatenar strings que se insertarán en el pipe
char* concat(char *s1, char *s2)
{
    char *nuevoString = malloc(strlen(s1) + strlen(s2) + 2);
    strcpy(nuevoString, s1);
    strcat(nuevoString, s2);
    strcat(nuevoString, "\n");
    return nuevoString;
}

//metodo auxiliar para concatenar string y char que se insertará en el pipe
char* concatChar(char *s1, char s2)
{
    char *nuevoString = malloc(strlen(s1) + 3);
    strcpy(nuevoString, s1);
    strncat(nuevoString, &s2, 1);
    strcat(nuevoString, "\n");
    return nuevoString;
}