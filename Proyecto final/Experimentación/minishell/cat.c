#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h> 
#include <stddef.h>

// Cat
int main(int argc,char** argv){

	if(argv[1][0]=='\n'){
       		fprintf(stderr, "Error con los argumentos pasados\n");
        	return 0;
    	}
    
    FILE* arch;
	char BUFFER[255];
	arch=fopen(argv[1],"r");

	if ( arch != NULL){
		printf("Archivo %s:\n",argv[1]);
		
		while (!feof(arch)){
			if (fgets(BUFFER,255,arch)!=NULL){
				printf("%s \n",BUFFER);
			}
		}
		fclose(arch);			
	}
	else{
		printf("Error al leer el archivo. \n");
	}

	return 0;
}
