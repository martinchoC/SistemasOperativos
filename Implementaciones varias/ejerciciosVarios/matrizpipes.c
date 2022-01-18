#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

//Llenamos la matriz con valores aleatorios entre 0 y 10
void cargarValores(int n, int m[][n])
{
	int i, j;
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			m[i][j]=rand()%11;
}

//Imprimimos la matriz por pantalla
void imprimir(int n, int m[][n])
{
	int i, j;
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
			printf("%d ", m[i][j]);
		printf("\n");
	}
	printf("\n");
}

//Programa ppal
int main()
{
	//Pedimos que se ingrese el tamaño de las matrices
	int n;
	printf("Ingrese un valor de N entre 4 y 6 para la cantidad de filas y columnas de cada matriz NxN: ");
	scanf("%d", &n);
	if(n < 4 || n > 6)
	{
		printf("El número ingresado debe estar en el rango [4, 6]\n");
		exit(-1);
	}

	//Creamos la tuberia
	int estado;
	int tuberia[2];
	pipe(tuberia);

	//Creamos las matrices de entrada
	int m1[n][n];
	cargarValores(n, m1);
	int m2[n][n];
	cargarValores(n, m2);

	//Creamos la matriz de salida
	int salida[n][n];
	
	//imprimimos las matrices 
	printf("\nPrimer matriz:\n");
	imprimir(n, m1);
	printf("Segunda matriz:\n");
	imprimir(n, m2);

	//Calculamos el producto entre las dos matrices
	int fila, columna, puntero;
	for(fila = 0; fila < n; fila++)
	{
		//Creamos un procesos hijo por fila
		pid_t pid = fork();
		int resultado = 0;

		//Si estamos en un hijo resolvemos la multiplicacion
		if(pid == 0)
		{
			int vector[n];
			close(tuberia[0]);
			for(columna = 0; columna < n; columna++)
			{
				for(puntero = 0; puntero < n; puntero++)
					resultado += m1[fila][puntero] * m2[puntero][columna];
				vector[columna] = resultado;
				resultado = 0;
			}
			write(tuberia[1], vector, n * sizeof(int));
			printf("PID: %d,PPID: %d, Fila calculada: %d\n",getpid(), getppid(), fila);
			exit(fila);
		}
	}
	
	//Armamos la salida
	int filaAux[n];
	for(fila = 0; fila < n; fila++)
	{
		wait(&estado);
		read(tuberia[0], filaAux, n * sizeof(int));
		int estadoHijo = WEXITSTATUS(estado);
		int col;
		for(col = 0; col < n; col++)
			salida[estadoHijo][col]= filaAux[col];		
	}

	//Imprimimos por pantalla
	printf("\nResultado:\n");
	imprimir(n, salida);
	exit(0);
}

