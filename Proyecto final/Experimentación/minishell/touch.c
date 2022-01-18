#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h> 
#include <stddef.h>

// Crear archivo.

int main(int argc,char** argv){
	if(argv[1][0]=='\n'){
       		fprintf(stderr, "Error con los argumentos pasados\n");
        	return 0;
    	}
	if (creat(argv[1],0700)!=0){
		printf("Se creó correctamente el archivo \n");	
	}
	else{
		printf("Error al crear el archivo. \n");
	}
	return 0;
}

