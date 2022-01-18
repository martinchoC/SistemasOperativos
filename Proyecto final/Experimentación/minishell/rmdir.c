#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h> 
#include <stddef.h>

// Borrar directorio.

int main(int argc,char** argv){

	if(argv[1][0]=='\n'){
       		fprintf(stderr,"Error al borrar directorio. \n");
        	return 0;
    	}
	if (rmdir(argv[1])==0){
		printf("Directorio eliminado. \n");	
	}
	else{
		printf("Error al eliminar el directorio. \n");
	}
	return 0;
}

