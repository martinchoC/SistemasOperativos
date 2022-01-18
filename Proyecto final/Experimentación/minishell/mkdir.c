#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h> 
#include <stddef.h>

// Crear directorio.

int main(int argc,char** argv){

	if(argv[1][0]=='\n'){
       		fprintf(stderr,"Error al crear directorio. \n");
        	return 0;
    	}
	if (mkdir(argv[1],0700)==0){
		printf("Creacion del directorio realizada. \n");	
	}
	else{
		printf("Error al crear el directorio. \n");
	}
	return 0;
}

