/*..................................................................*/
//				Castro, Martin - Vazquez, Santiago.
/*..................................................................*/


/*
*   gcc shell.c -o shell
*   gcc mkdir.c -o mkdir
*   gcc rmdir.c -o rmdir
*   gcc touch.c -o touch
*   gcc ls.c -o ls
*   gcc cat.c -o cat
*   gcc chmod.c -o chmod
*
*/

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h> 
#include <stddef.h>

/* TAMANO DE BUFFER DEFINIDO. */
/* NUMERO MAXIMO DE ARGUMENTOS. */
#define BUFFERSIZE 1024
#define ARGUMENTOS 5

/*Ruta del directorio actual.*/
char directorioActual[BUFFERSIZE];


void ayuda();

/* Funcion que separa las palabras ingresadas por consola */
char** separarStrings(char* comando);

int main(int argc,char** argv){   
    printf("************************************************************************************************* \n ");
	printf("Esta es la minishell de Linux, programada en Linux. si necesita ayuda, ingrese el comando 'ayuda' \n");
    printf("************************************************************************************************* \n");
 
    while(1){

        getcwd(directorioActual, sizeof(directorioActual));
        printf("[%s]$ ",directorioActual);
        
        char * BUFFER = malloc(sizeof(char)*BUFFERSIZE);
        fgets(BUFFER,1024,stdin);

        char** strings=separarStrings(BUFFER);
        pid_t pid;
        
        if(strcmp(strings[0],"mkdir")!=0 && strcmp(strings[0],"rmdir")!=0 && strcmp(strings[0],"touch")!=0 && strcmp(strings[0],"ls")!=0 && strcmp(strings[0],"cat")!=0 && strcmp(strings[0],"ayuda")!=0 && strcmp(strings[0],"chmod")!=0 && strcmp(strings[0],"exit")!=0){
            free(BUFFER);
            free(strings);
            printf("Error al ingresar comando. \n");
            free(BUFFER);
            free(strings);
        }
        else
        {
            if(strcmp(strings[0],"mkdir")==0){
                char* wd = strings[1];
                strings[0] = "./mkdir";
                pid = fork();

                if(pid == 0){
                    execv(strings[0],strings);
                }

                wait(NULL);
            }
            if(strcmp(strings[0],"rmdir")==0){
                char* wd = strings[1];
                strings[0] = "./rmdir";
                pid = fork();

                if(pid == 0){
                    execv(strings[0],strings);
                }

                wait(NULL);
            }

            if(strcmp(strings[0],"touch")==0){
                char* wd = strings[1];
                strings[0] = "./touch";
                pid = fork();

                if(pid == 0){
                    execv(strings[0],strings);
                }

                wait(NULL);
            }

            if(strcmp(strings[0],"ls")==0){
                char* wd = strings[1];
                strings[0] = "./ls";
                pid = fork();

                if(pid == 0){
                    execv(strings[0],strings);
                }

                wait(NULL);
            }
                        
            if(strcmp(strings[0],"cat")==0){
                char* wd = strings[1];
                strings[0] = "./cat";
                pid = fork();

                if(pid == 0){
                    execv(strings[0],strings);
                }

                wait(NULL);
            }
                                
            if(strcmp(strings[0],"ayuda")==0){
                ayuda();
            }
                                    
            if(strcmp(strings[0],"chmod")==0){
                char* wd = strings[1];
                strings[0] = "./chmod";
                pid = fork();

                if(pid == 0){
                    execv(strings[0],strings);
                }

                wait(NULL);
            }
                                        
            if(strcmp(strings[0],"exit")==0){
                printf("\e[1;1H\e[2J");
                exit (0);
            }
        }
    }
}


void ayuda(){
    printf("\n");
    printf("................................AYUDA.................................. \n");
    printf("\n");
    printf("Lista de comandos: \n");
    printf("mkdir <nombre del directorio>: Crea directorio con el nombre ingresado por parametro.\n");
    printf("rmdir <nombre del directorio>: Elimina directorio con el nombre pasado por parametro.\n");
    printf("touch <nombre del archivo>: Crea archivo con el nombre pasado por parametro. \n");
    printf("ls <nombre del directorio>: Muestra una lista del contenido de un directorio pasado por parametro. \n");
    printf("cat <nombre del archivo>: Muestra el contenido de un archivo pasado por parametro. \n");
    printf("chmod <nombre del archovo><permisos>: Modifica los permisos de un archivo. \n");
    printf("exit: Finaliza la ejecucion de consola. \n ");
    printf("\n");
}

char ** separarStrings(char * comando){
    
    char ** arg = malloc(sizeof(char*)*ARGUMENTOS);
    char * argv,*argm;
    char delim[] = " \t\r\n\a";
    int i,j;

    if (!arg) {
        fprintf(stderr,"Error con la memoria dinamica\n");
        exit(EXIT_FAILURE);
    }   

    j = 0;

    for (argv = strtok(comando, delim); argv; argv = strtok(NULL, delim)){

        argm = malloc(sizeof(char)*1024);
        
        if(!argm){
            fprintf(stderr,"Error con la memoria dinamica\n");
            exit(EXIT_FAILURE);
        }

        i = 0;
        
        while(argv[i]!='\0'){
            argm[i] = argv[i];
            i++;
        }
        argm[i] =argv[i];
        
        arg[j]=argv;
        j++;
    }

    argv = malloc(sizeof(char));
    argv[0]='\n';
    arg[j]=argv;
    return arg;
}