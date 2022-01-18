#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

mode_t mu = 0, mg = 0, mo = 0;

int validarEntrada(char *file, char *data) {
	
	char *user[] ={"usuario", "grupo", "otros"};
	
	// Verifica si ingresaron las dos entradas
	if((file == NULL) || (data == NULL)) {
        printf("Parametros incorrectos\n");
		return 0;
    }
	
	//Verifica que la longitud del nro ingresado sea igual a 3
	if(strlen(data) > 3) {
        printf("El numero que representa los privilegios debe ser de 3 digitos. \n");
		return 0;
	}
	
	int i;
	//Verifica que los privilegios ingresados para el usuario sean validos
	for(i=0; i<3; i++)
		if((data[i]  < '0') || (data[i] > '7')) {
			printf("Los privilegios ingresados para el %s no son validos, debe ser entre 0 y 7. \n", user[i]);
			return 0;
		}
		
	return 1;

}

int main(int argc, char * argv[]) {
	char datos[3], mode[3]= "0";
	int privilegios;
	
	if(validarEntrada(argv[1], argv[2])){
	
	//Copia a strings para facilitar la lectura
	strcpy(datos, argv[2]);
	
	//Concatena los strings y genera un parametro octal para la funcion chmod
	strcat(mode, datos);
	privilegios = strtol(mode, 0, 8);
		
	if(chmod(argv[1], privilegios) == -1){
		perror("Error a la hora de asignar privilegios");
		return -1;
	}
	else
		printf("Los privilegios para el archivo %s ahora son %s.\n", argv[1], argv[2]);
	}
	else // no se pudo validar la entrada
		return -1;
	
	return 0;
}