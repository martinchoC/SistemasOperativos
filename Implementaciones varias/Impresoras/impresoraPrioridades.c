#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include<time.h>

#define MAX_USERS 15

pthread_mutex_t * printer1;
pthread_mutex_t * printer2;
sem_t * disponibles;

typedef struct celda{
	struct usuar * user;
	struct celda * next;
	struct celda * prev;
}*usersList;

typedef struct usuar{
	pthread_t u;
	int prioridad;
}*usuario;

int insertarOrdenado(usersList * listaU, usuario u){
	usersList newNode = (usersList)malloc(sizeof(struct celda));
	newNode->user=u;
	if((*listaU)==NULL){
		newNode->next=NULL;
		(*listaU)=newNode;
		return 0;
	}
	else{
		int continuar = 1;
		usersList aux = (*listaU);
		while(aux->next != NULL && continuar == 1){
			if(aux->user->prioridad < newNode->user->prioridad)
				aux = aux->next;
			else
				if(aux->user->prioridad > newNode->user->prioridad)
					continuar = 0;
				else{ 
					return -1;
					continuar = 0; 
				}
		}

		if(aux->prev == NULL && aux->next == NULL){
			if(aux->user->prioridad < newNode->user->prioridad){
				aux->next=newNode;
				newNode->prev=aux;
				newNode->next=NULL;
			}
			else 
				if(aux->user->prioridad > newNode->user->prioridad){
					newNode->next=aux;
					newNode->prev=NULL;
					aux->prev=newNode;
					(*listaU)=newNode;
				}

		}
		else 
			if(aux->prev != NULL && aux->next != NULL){
				if(aux->user->prioridad < newNode->user->prioridad){
					newNode->prev = aux;
					newNode->next = aux->next;
					aux->next->prev=newNode;
					aux->next=newNode;
				}
			else 
				if(aux->user->prioridad > newNode->user->prioridad){
					newNode->next=aux;
					newNode->prev = aux->prev;
					aux->prev->next=newNode;
					aux->prev=newNode;
				}	

			}
			else{
				if(aux->prev != NULL && aux->next == NULL){
					if(aux->user->prioridad < newNode->user->prioridad){
						newNode->prev=aux;
						newNode->next=NULL;
						aux->next=newNode;
					}
					else 
						if(aux->user->prioridad > newNode->user->prioridad){
							newNode->prev=aux->prev;
							newNode->next=aux;
							aux->prev->next=newNode;
							aux->prev=newNode;
						}

				}
				else{
					if(aux->user->prioridad < newNode->user->prioridad){
						newNode->prev=aux;
						newNode->next=aux->next;
						aux->next->prev = newNode;
						aux->next=newNode;
					}
					else 
						if(aux->user->prioridad > newNode->user->prioridad){
							newNode->prev=NULL;
							newNode->next=aux;
							aux->prev=newNode;
							(*listaU)=newNode;
						}

				}

			}
	}
	return 0;
}

void showPriorities(usersList lista){
	int i=1;
	printf("Lista de prioridades\n");
	while(lista!=NULL){
		printf(" #%i-  %i \n",i,lista->user->prioridad);
		lista = lista->next;
		i++;
	}
}

int requerir(){
	sem_wait(disponibles);
	int printer=0;
	if(pthread_mutex_trylock(printer1)==0)
		printer = 1;
	else if(pthread_mutex_trylock(printer2)==0)
		printer = 2;
	return printer;
}

void liberar(int id){
	if(id == 0){
		pthread_mutex_unlock(printer1);
		printf("Impresora %d liberada\n", id);
		sem_post(disponibles);
	}
	else{
		pthread_mutex_unlock(printer2);
		printf("Impresora %d liberada\n", id);
		sem_post(disponibles);
	}
	pthread_exit(0); 
}

void * begin(void * p){
	int id;
	int i=0;
	while(i<MAX_USERS){
		id = requerir();
		printf("La impresora %i fue ocupada\n", id);
		sleep(1);
		printf("La impresora %i está imprimiendo\n", id);
		liberar(id);
		i++;
	}
	return NULL;
}

int generarPrioridad(){
	int r = rand()%1000;
	return r;
}

void inicializarUsuarios(usuario * arrayDeUsuarios){
	int i;
	for(i=0;i<MAX_USERS;i++){
		arrayDeUsuarios[i]=(usuario)malloc(sizeof(struct usuar));
		arrayDeUsuarios[i]->u = (pthread_t)malloc(sizeof(pthread_t));
		arrayDeUsuarios[i]->prioridad=generarPrioridad();
	}
}

void ejecutar(usersList uList){
	while(uList!=NULL){
		pthread_create(&uList->user->u,NULL,begin,NULL);
		uList=uList->next;
	}
}

void main(){

	int i=0;
	srand(time(NULL));
	usuario arrayDeUsuarios [MAX_USERS];
	inicializarUsuarios(arrayDeUsuarios);
	usersList listOfUsers = NULL;
	
	//insercion de usuarios segun prioridad
	while(i<MAX_USERS){
		insertarOrdenado(&listOfUsers,arrayDeUsuarios[i]);
		i++;
	}

	printf("Las prioridades de los usuarios que desean imprimir son:\n");
	showPriorities(listOfUsers);

	disponibles = (sem_t *) malloc(sizeof(sem_t));
	printer1 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	printer2 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_unlock(printer1);
	pthread_mutex_unlock(printer2);
	sem_init(disponibles,0,2);

	ejecutar(listOfUsers);

	while(listOfUsers!=NULL){
		pthread_join(listOfUsers->user->u,NULL);
		listOfUsers=listOfUsers->next; 
	}

	free(printer1);
	free(printer2);
	free(disponibles);

	for(i=0;i<MAX_USERS;i++){
		usuario aux = arrayDeUsuarios[i];
		free(aux);
	}
}