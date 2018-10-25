#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
//#include "queue.h"
#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 10
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET ADDSTRLEN
#define MIN_NUM_ARG 5
struct thread_arguments{
	int array_index;
	// struct queue *t_q;
	
	FILE *serviced;
	int num_serviced; 

	char **t_files;
	int num_threads;
	int thread_num;

	char **arr;
	int *front_array;
	FILE *results;
	bool *req_exist;
	int *en;
	int *de;
	pthread_mutex_t *lock; 
	pthread_mutex_t *sda; 
	pthread_mutex_t *service_file; 
	pthread_mutex_t *result_file;
	sem_t *items_avail;
	sem_t *space_avail;

};
struct resolver_arguments{
	// struct queue *t_q;

};


void requester_func(struct thread_arguments *requester_threads);
void resolver_func(struct thread_arguments *resolver_threads);