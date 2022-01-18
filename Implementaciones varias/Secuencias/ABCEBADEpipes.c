#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int pipefdAB[2]; //Pipe entre A->B
int pipefdBA[2]; //Pipe entre B->A
int pipefdBC[2]; //Pipe entre B y C
int pipefdCE[2]; //Pipe entre C y E
int pipefdEB[2]; //Pipe entre E y B
int pipefdAD[2]; //Pipe entre A y D
int pipefdDE[2]; //Pipe entre D y E
int pipefdEA[2]; //Pipe entre E y A

int buf;
int aparicion; //Cada proceso tendra una copia propia de aparicion

void procesoA();
void procesoB();
void procesoC();
void procesoD();
void procesoE();

/* 	Secuencia ABCEBADE...
	Pipes: A<->B, B->C, C->E, E->B, A->D, D->E, E->A */
int main(int argc, char *argv[])
{    	
	pid_t pidB;
	pid_t pidC;
	pid_t pidD;
	pid_t pidE;

    if (pipe(pipefdAB) == -1 || pipe(pipefdBA) == -1 ||pipe(pipefdBC) == -1 || 
	pipe(pipefdCE) == -1 || pipe(pipefdEB) == -1 || pipe(pipefdAD) == -1 || 
	pipe(pipefdAD) == -1 || pipe(pipefdDE) == -1 || pipe(pipefdEA) == -1) {
		perror("pipe AB");
		exit(EXIT_FAILURE);
    }

    pidB = fork();
    if (pidB == -1) {
        perror("fork B");
        exit(EXIT_FAILURE);
    }
    if (pidB == 0) {  /* Proceso B  */
	/* Creo el proceso c */
	pidC = fork();
	if (pidC == -1) { //Error fork C
		perror("fork C");
		exit(EXIT_FAILURE);
    	}

    	if (pidC == 0) { /* Proceso C  */
		/* Creo el proceso E */
		pidE = fork();
		if (pidE == -1) { //Error fork E
			perror("fork E");
			exit(EXIT_FAILURE);
	    	}

	    	if (pidE == 0) { /* Proceso E  */
			/* Creo el proceso D */
			pidD = fork();
			if (pidD == -1) { //Error fork D
				perror("fork D");
				exit(EXIT_FAILURE);
		    	}

		    	if (pidD == 0) { /* Proceso D  */
				procesoD();
				_exit(EXIT_SUCCESS);
			}	
			else { /* Proceso E  */
				procesoE();
				_exit(EXIT_SUCCESS);
			}	
		}	
		else { /* Proceso C  */
			procesoC();
			_exit(EXIT_SUCCESS);
		}		
	}	
	else { /* Proceso B  */
		procesoB();
		_exit(EXIT_SUCCESS);
	}
    } else {  /* Proceso A */
      	procesoA();
        exit(EXIT_SUCCESS);
    }
}

void procesoA(){
	aparicion = 1;
	close(pipefdAB[0]); /* Cierro la lectura en AB */
	close(pipefdBA[1]); /* Cierro la escritura en BA */
	close(pipefdEA[1]); /* Cierro la escritura en EA */
	close(pipefdAD[0]); /* Cierro la lectura en AD */
	close(pipefdBC[0]);close(pipefdBC[1]);
	close(pipefdCE[0]);close(pipefdCE[1]);
	close(pipefdEB[0]);close(pipefdEB[1]);
	close(pipefdDE[0]);close(pipefdDE[1]);
	while(1){
		printf("A\n");
		if(aparicion == 1){ //Es la primera A, escribir a B
			write(pipefdAB[1], &aparicion, sizeof(aparicion)); //Escribo a B
			read(pipefdBA[0], &buf, sizeof(buf)); //Espero a B
			aparicion = 2; //indico que tiene que imprimirse la segunda (A o B)
		}
		else{ //La segunda A ya se imprimio, escribe a D
			aparicion = 1;
			write(pipefdAD[1], &aparicion, sizeof(aparicion)); //Escribo a D
			read(pipefdEA[0], &buf, sizeof(buf)); //Espero a E
			sleep(1); //Para que no escriba A antes que E
		}
	}
}
	
void procesoB(){
	aparicion = 1;
	close(pipefdAB[1]); /* Cierro la escritura en AB */
	close(pipefdBC[0]); /* Cierro la lectura en BC */
	close(pipefdEB[1]); /* Cierro la escritura en EB */
	close(pipefdBA[0]); /* Cierro la lectura en BA */
	close(pipefdEA[0]);close(pipefdEA[1]);
	close(pipefdAD[0]);close(pipefdAD[1]);
	close(pipefdCE[0]);close(pipefdCE[1]);
	close(pipefdDE[0]);close(pipefdDE[1]);
	while(1){
		if(aparicion == 1){ //Es la primera B, esperar que escriba A
			read(pipefdAB[0], &buf, sizeof(buf)); //Espero a A
			aparicion = 2; //indico que tiene que imprimirse la segunda B
		}
		else{ //Es la segunda aparicion de B; antes escribir a C y esperar a E; luego escribir a A
			write(pipefdBC[1], &aparicion, sizeof(aparicion)); //Escribo a C
			read(pipefdEB[0], &buf, sizeof(buf)); //Espero a E
			write(pipefdBA[1], &aparicion, sizeof(aparicion)); //Escribo a A
			aparicion = 1;
		}
		printf("B\n");
	}
}

void procesoC(){
	aparicion = 1;
	close(pipefdBC[1]);  /* Cierro la escritura en BC */
	close(pipefdCE[0]);          /* Cierro la lectura en CE */
	close(pipefdAB[0]);close(pipefdAB[1]);
	close(pipefdEB[0]);close(pipefdEB[1]);
	close(pipefdBA[0]);close(pipefdBA[1]);
	close(pipefdEA[0]);close(pipefdEA[1]);
	close(pipefdAD[0]);close(pipefdAD[1]);
	close(pipefdDE[0]);close(pipefdDE[1]);
	while(1){
		read(pipefdBC[0], &buf, sizeof(buf)); //Espero a B
		
		printf("C\n");
	
		write(pipefdCE[1], &aparicion, sizeof(aparicion)); //Escribo a E
	}
}

void procesoD(){
	aparicion = 1;
	close(pipefdAD[1]);  /* Cierro la escritura en AD */
	close(pipefdDE[0]);          /* Cierro la lectura en DE */
	close(pipefdBC[0]);close(pipefdBC[1]);
	close(pipefdCE[0]);close(pipefdCE[1]);
	close(pipefdAB[0]);close(pipefdAB[1]);
	close(pipefdEB[0]);close(pipefdEB[1]);
	close(pipefdBA[0]);close(pipefdBA[1]);
	close(pipefdEA[0]);close(pipefdEA[1]);
	while(1){
		read(pipefdAD[0], &buf, sizeof(buf)); //Espero a A
		
		printf("D\n");
	
		write(pipefdDE[1], &aparicion, sizeof(aparicion)); //Escribo a E
	}
}
   	
void procesoE(){
	aparicion = 1;
	close(pipefdCE[1]);  /* Cierro la escritura en CE */
	close(pipefdEB[0]);          /* Cierro la lectura en EB */
	close(pipefdDE[1]);          /* Cierro la escritura en DE */
	close(pipefdEA[0]);          /* Cierro la lectura en EA */
	close(pipefdAD[0]);close(pipefdAD[1]); 
	close(pipefdBC[0]);close(pipefdBC[1]);
	close(pipefdAB[0]);close(pipefdAB[1]);
	close(pipefdBA[0]);close(pipefdBA[1]);
	while(1){
		if(aparicion == 1){ //Es la primera E, esperar que escriba C
			read(pipefdCE[0], &buf, sizeof(buf)); //Espero a C
			aparicion = 2; //indico que tiene que imprimirse la segunda E
		}
		else{ //Es la segunda aparicion de E; antes escribir a B y esperar a D; luego escribir a A
			write(pipefdEB[1], &aparicion, sizeof(aparicion)); //Escribo a B
			read(pipefdDE[0], &buf, sizeof(buf)); //Espero a D
			write(pipefdEA[1], &aparicion, sizeof(aparicion)); //Escribo a A
			aparicion = 1; 
		}		
		printf("E\n\n");

	}
}

