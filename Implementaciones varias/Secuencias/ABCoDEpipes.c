#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Secuencia AB(C o D)E...

int A[2], B[2], X[2], E[2];

void procesoA();
void procesoB();
void procesoC();
void procesoD();
void procesoE();

int main()
{	
	pipe(A);
	pipe(B);
	pipe(X);
	pipe(E);
	int i; 

    if (pipe(A) == -1 || pipe(B) == -1 || pipe(X) == -1 || pipe(E) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
    }

	char buffer = 'A';
	write(A[1], &buffer, 1);
	
	pid_t pid1=fork();
	if(pid1==0)	//PROCESO A
	{	
		procesoA();
	}
	
	pid_t pid2=fork();
	if(pid2==0)	//PROCESO B
	{
        procesoB();
	}
	
	pid_t pid3=fork();
	if(pid3==0)	//PROCESO C
	{
        procesoC();
	}
			
	pid_t pid4=fork();
	if(pid4==0)	//PROCESO D
	{	
        procesoD();
	}
	
	pid_t pid5=fork();
	if(pid5==0)	//PROCESO E
	{
		procesoE();
	}
	
	for(i=0;i<5;i++)
		wait(NULL);
		
	return 0;
}

void procesoA() {
    close(B[0]);
    close(X[0]);
    close(E[0]);
    close(A[1]);
    close(X[1]);
    close(E[1]);
    char buffer;
    while(1)
    {
        read(A[0], &buffer, 1);
        printf("%c",buffer);
        fflush(stdout);
        buffer='B';
        write(B[1], &buffer, 1);
    }
}

void procesoB() {
    close(A[0]);
    close(X[0]);
    close(E[0]);
    close(A[1]);
    close(B[1]);
    close(E[1]);
    char buffer;
    while(1)
    {
        read(B[0], &buffer, 1);
        printf("%c",buffer);
        fflush(stdout);
        buffer='C';
        write(X[1], &buffer, 1);    }
}

void procesoC() {
    close(A[0]);
    close(B[0]);
    close(E[0]);
    close(A[1]);
    close(X[1]);
    close(B[1]);
    char buffer;
    while(1)
    {
        read(X[0], &buffer, 1);
        printf("%c",buffer);
        fflush(stdout);
        buffer='E';
        write(E[1], &buffer, 1);
    }
}

void procesoD() {
    close(A[0]);
    close(B[0]);
    close(E[0]);
    close(A[1]);
    close(B[1]);
    close(X[1]);
    char buffer;
    while(1)
    {
        read(X[0], &buffer, 1);
        printf("%c",buffer);
        fflush(stdout);
        buffer='E';
        write(E[1], &buffer, 1);
    }
}

void procesoE() {
    close(A[0]);
    close(B[0]);
    close(X[0]);
    close(B[1]);
    close(X[1]);
    close(E[1]);
    char buffer;
    while(1)
    {
        read(E[0], &buffer, 1);
        printf("%c\n",buffer);
        fflush(stdout);
        buffer='A';
        write(A[1], &buffer, 1);
    }
}