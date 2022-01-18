#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void fibonacci (int n){
    int n1=1;
    int n2=0;
    int fib=0;
    int i=0;
        for (i=0; i<n; i++){
            fib= n1 + n2;
            n1=n2;
            n2=fib;
            printf("%i \n",fib);
        }
    FILE *arch;
    arch = fopen("fib","w");
    fwrite (&fib,sizeof(int),1,arch);
    fclose (arch);
    }


int main()
{
    int n;
    scanf("%i" , &n);
    if(n>=0){
        int child = fork();
        if (child==0){
            fibonacci(n);
        }else{
            wait(NULL);
            int *f=malloc(sizeof(int));
            FILE *arch = fopen("fib","r");
            fread (f, sizeof(int),1,arch);
            fclose(arch);
            printf("ultimo termino de la serie: %i \n", *f);
            printf("proces ID del Hijo: %i \n" , child);
            }
    }else printf("el numero ingresado es negativo");
    return 1;
}
