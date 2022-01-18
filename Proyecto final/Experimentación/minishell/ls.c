#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc > 1){
        DIR * directorio = opendir(argv[1]); //abre el archivo para lectura
        if (directorio==NULL) {
            printf("Error al abrir el directorio.\n");
            exit(EXIT_FAILURE);
        }
        else{
            struct dirent * elem = readdir(directorio);
            while(elem!=NULL){
                printf("%s \n",elem->d_name);
                elem = readdir(directorio);
            }
            closedir(directorio);
            printf("\n");
        }
    }
    else{
		printf("Error: no se especificó ningún directorio.\n");
		exit(EXIT_FAILURE);
    }

}