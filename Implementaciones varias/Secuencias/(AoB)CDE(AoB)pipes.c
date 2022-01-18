#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int pipefdABAB[2]; //Pipe entre (A o B) y (A o B)
int pipefdABC[2]; //Pipe entre (A o B) y C
int pipefdCD[2]; //Pipe entre C y D
int pipefdDE[2]; //Pipe entre D y E
int pipefdEAB[2]; //Pipe entre E y (A o B)

int buf;
int ab = 0;

void procesoA();
void procesoB();
void procesoC();
void procesoD();
void procesoE();

/* 	Secuencia (AoB)CDE(AoB)... con pipes.
	Pipes: (AoB)->(AoB); (AoB)->C; C->D; D->E; E->(AoB) */
int main(int argc, char *argv[])
{    	
	pid_t pidB;
	pid_t pidC;
	pid_t pidD;
	pid_t pidE;

    if (pipe(pipefdABAB) == -1 || pipe(pipefdABC) == -1 || pipe(pipefdCD) == -1 || 
	pipe(pipefdDE) == -1 || pipe(pipefdEAB) == -1 ) {
		perror("pipe");
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
	ab = 1;
	write(pipefdABAB[1], &ab, sizeof(ab)); //Escribo a (A o B)
      	procesoA();
        exit(EXIT_SUCCESS);
    }
}

void procesoA(){
	close(pipefdABC[0]); /* Cierro la lectura en AB->C */
	close(pipefdEAB[1]); /* Cierro la escritura en E->AB */
	close(pipefdCD[0]); close(pipefdCD[1]);/* Cierro la lectura y escritura en C->D */
	close(pipefdDE[0]); close(pipefdDE[1]);/* Cierro la lectura y escritura en D->E */
	while(1){	
		read(pipefdABAB[0], &buf, sizeof(buf));	
		printf("A\n");
		if(buf == 1){ //es la primera A
			write(pipefdABC[1], &ab, sizeof(ab)); //Escribo a C
			read(pipefdEAB[0], &buf, sizeof(buf)); //Espero a E
			ab = 2; //indico que tiene que imprimirse la segunda (A o B)
		}else if(buf == 2){ //La segunda (A o B) ya se imprimio, tiene que imprimirse la tercera (A o B)
			ab = 3;
			}
			else{ //La tercera (A o B) ya se imprimio, vuelvo a escribir a C, espero a E, y vuelvo a empezar
				write(pipefdABC[1], &ab, sizeof(ab)); //Escribo a C
				read(pipefdEAB[0], &buf, sizeof(buf)); //Espero a E
				ab = 1; //indico que tiene que imprimirse la primera (A o B)
				printf("\n");
				sleep(1);
			}
		write(pipefdABAB[1], &ab, sizeof(ab)); //Escribo a (A o B)
	}
}

void procesoB(){
	close(pipefdABC[0]); /* Cierro la lectura en AB->C */
	close(pipefdEAB[1]); /* Cierro la escritura en E->AB */
	close(pipefdCD[0]); close(pipefdCD[1]);/* Cierro la lectura y escritura en C->D */
	close(pipefdDE[0]); close(pipefdDE[1]);/* Cierro la lectura y escritura en D->E */
	while(1){	
		read(pipefdABAB[0], &buf, sizeof(buf));	
		printf("B\n");
		if(buf == 1){ //es la primera B
			write(pipefdABC[1], &ab, sizeof(ab)); //Escribo a C
			read(pipefdEAB[0], &buf, sizeof(buf)); //Espero a E
			ab = 2;  //indico que tiene que imprimirse la segunda (A o B)
		}else if(buf == 2){ //La segunda (A o B) ya se imprimio, tiene que imprimirse la tercera (A o B)
			ab = 3;
			}
			else{ //La tercera (A o B) ya se imprimio, vuelvo a escribir a C, espero a E, y vuelvo a empezar
				write(pipefdABC[1], &ab, sizeof(ab)); //Escribo a C
				read(pipefdEAB[0], &buf, sizeof(buf)); //Espero a E
				ab = 1; //indico que tiene que imprimirse la primera (A o B)
				printf("\n");
				sleep(1);
			}
		write(pipefdABAB[1], &ab, sizeof(ab)); //Escribo a (A o B)
	} 
}

void procesoC(){
	close(pipefdABC[1]);  /* Cierro la escritura en AB->C */
	close(pipefdCD[0]);  /* Cierro la lectura en C->D */
	close(pipefdDE[0]); close(pipefdDE[1]);/* Cierro la lectura y escritura en D->E */
	close(pipefdEAB[0]); close(pipefdEAB[1]);  /* Cierro la lectura y escritura en E->AB */
	close(pipefdABAB[0]); close(pipefdABAB[1]);/* Cierro la lectura y escritura en AB->AB */
	while(1){
		read(pipefdABC[0], &buf, sizeof(buf)); //Espero a (A o B)
		
		printf("C\n");
	
		write(pipefdCD[1], &ab, sizeof(ab)); //Escribo a E
	}
}

void procesoD(){
	close(pipefdCD[1]);  /* Cierro la escritura en C->D */
	close(pipefdDE[0]);  /* Cierro la lectura en D->E */
	close(pipefdEAB[0]); close(pipefdEAB[1]);  /* Cierro la lectura y escritura en E->AB */
	close(pipefdABC[0]); close(pipefdABC[1]);  /* Cierro la lectura y escritura en AB->C */
	close(pipefdABAB[0]); close(pipefdABAB[1]);/* Cierro la lectura y escritura en AB->AB */	
	while(1){
		read(pipefdCD[0], &buf, sizeof(buf)); //Espero a C
		
		printf("D\n");
	
		write(pipefdDE[1], &ab, sizeof(ab)); //Escribo a E
	}
}
   	
void procesoE(){
	int i = 0;
	close(pipefdDE[1]);  /* Cierro la escritura en D->E */
	close(pipefdEAB[0]); /* Cierro la lectura en E->B */
	close(pipefdABC[0]); close(pipefdABC[1]);  /* Cierro la lectura y escritura en AB->C */	
	close(pipefdCD[0]); close(pipefdCD[1]);/* Cierro la lectura y escritura en C->D */
	close(pipefdABAB[0]); close(pipefdABAB[1]);/* Cierro la lectura y escritura en AB->AB */
	while(1){
		read(pipefdDE[0], &buf, sizeof(buf)); //Espero a D
		
		printf("E\n");

		sleep(1);
		write(pipefdEAB[1], &ab, sizeof(ab)); //Escribo a (A o B)
	}
}
