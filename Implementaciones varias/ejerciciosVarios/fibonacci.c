#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>  

int fib(int n)
{
	if(n<2)
		return 1;
	else return fib(n-1)+fib(n-2);
}

int main(int argc, char** argv)
{
	int n=atoi(argv[1]);	
	if(n<0)
	{
		printf("El número ingresado no debe ser negativo\n");
		exit(-1);
	}
	FILE *f;
	pid_t pid=fork();
	
	if(pid==0)
	{
		int i=0;
		f=fopen("res_ej1.txt", "w");
		printf("Serie: ");
		while(i <= n)
		{
			int e= fib(i);
			fprintf(f, "%d ", e);
			printf("%d ", e);
			i++;
		}
		fclose(f);
		exit(0);	
	}
	else
	{
		wait(NULL);
		f=fopen("res_ej1.txt", "r");
		int ult;
		while(!feof(f))
			fscanf(f, "%d ", &ult);
		printf("\nUltimo valor de la serie: %d\n", ult);
		fclose(f);
		exit(0);
	}
	return 0;
	
}
