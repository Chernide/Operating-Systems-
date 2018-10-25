#include "multi_lookup.h"
#include "util.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 10
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET ADDSTRLEN
#define MIN_NUM_ARG 5
#define MAX_ARRAY_SIZE 50


void resolver_func(struct thread_arguments *resolver_threads){
	if(*resolver_threads->en != *resolver_threads->de){
		sem_wait(resolver_threads->items_avail);
	}
	pthread_mutex_lock(resolver_threads->sda);
	while(*resolver_threads->front_array != -1){
		char *n = resolver_threads->arr[*resolver_threads->front_array];
		*resolver_threads->de += 1;
		char oneIP[20];
		if(dnslookup(n, oneIP, 20) == UTIL_FAILURE){
			printf("Error: DNS failed lookup on hostname (%s)\n",n);
			strcpy(oneIP, "");
		}
		pthread_mutex_lock(resolver_threads->result_file);
		fprintf(resolver_threads->results, "%s,%s\n", n,oneIP);
		pthread_mutex_unlock(resolver_threads->result_file);
		printf("DEQUEUED: %s\n", n);
		free(resolver_threads->arr[*resolver_threads->front_array]);
		*resolver_threads->front_array = *resolver_threads->front_array - 1;
	}
	pthread_mutex_unlock(resolver_threads->sda);
	sem_post(resolver_threads->space_avail);
	pthread_mutex_lock(resolver_threads->lock);

	if( *resolver_threads->req_exist){
		pthread_mutex_unlock(resolver_threads->lock);
		resolver_func(resolver_threads);
	} else {
		pthread_mutex_unlock(resolver_threads->lock);
	}

}
void requester_func(struct thread_arguments *requester_threads){
	FILE *curr_file = fopen(requester_threads->t_files[requester_threads->array_index], "r");
	if(!curr_file){
		pthread_mutex_lock(requester_threads->service_file);
		printf("ARG! THERE ISN'T A FILE LAD\n");
		requester_threads->num_serviced++;
		fprintf(requester_threads->serviced, "Thread %d serviced %d files\n",requester_threads->thread_num, requester_threads->num_serviced);
		pthread_mutex_unlock(requester_threads->service_file);
	} else {
		char line[MAX_NAME_LENGTH];
		printf("Opened file (%s)\n", requester_threads->t_files[requester_threads->array_index]);
		while(fgets(line, sizeof(line), curr_file) != NULL){
			sem_wait(requester_threads->space_avail);
			line[strlen(line)-1] = '\0';
			pthread_mutex_lock(requester_threads->sda);
			if(*requester_threads->front_array < MAX_ARRAY_SIZE - 1){
				*requester_threads->front_array = *requester_threads->front_array + 1;
				// char *element = strdup(line);
				requester_threads->arr[*requester_threads->front_array] = strdup(line);
				*requester_threads->en += 1;
				// free(element);
			}
			pthread_mutex_unlock(requester_threads->sda);
			sem_post(requester_threads->items_avail);
		}
		fclose(curr_file);
	}
	if(requester_threads->array_index + requester_threads->num_threads < MAX_INPUT_FILES){
		requester_threads->array_index += requester_threads->num_threads;
		requester_func(requester_threads);
	}
}

int main(int argc, char *argv[]){
	struct timeval start; 
	struct timeval end;
	gettimeofday(&start, NULL);
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
	pthread_mutex_t sda = PTHREAD_MUTEX_INITIALIZER; 
	pthread_mutex_t service_file = PTHREAD_MUTEX_INITIALIZER; 
	pthread_mutex_t result_file = PTHREAD_MUTEX_INITIALIZER;
	sem_t items_avail;
	sem_t space_avail;
	sem_init(&space_avail, 0, MAX_ARRAY_SIZE);
	sem_init(&items_avail, 0, 0);
	pthread_mutex_init(&sda,NULL);
	int e = 0; 
	int d = 0;
	char **array = malloc(MAX_ARRAY_SIZE * sizeof(char*));
	int y;
	for(y = 0; y < MAX_ARRAY_SIZE; y++){
		array[y] = "";
	}

	if(argc - 1 < MIN_NUM_ARG){
		fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
		return -1;
	}
	int num_requester = atoi(argv[1]);
	if(num_requester > MAX_REQUESTER_THREADS){
		printf("Reseting number of requester threads to maximum, %d\n", MAX_REQUESTER_THREADS);
		num_requester = MAX_REQUESTER_THREADS;
	}
	int num_resolver = atoi(argv[2]);
	if(num_resolver > MAX_RESOLVER_THREADS){
		printf("Reseting number of requester threads to maximum, %d\n", MAX_RESOLVER_THREADS);
		num_resolver = MAX_RESOLVER_THREADS;	
	}
	printf("Number of requesters: %d\n",num_requester);
	printf("Number of resolvers: %d\n",num_resolver);
	int num_files = argc;

	FILE *service_log = fopen(argv[3], "w");
	FILE *result_log = fopen(argv[4], "w");
	if(!service_log){
		printf("Service log failed to open.\n");
		return -1;
	}
	if(!result_log){
		printf("Result log failed to open.\n");
		return -1;
	}
	char **fileNames = malloc(MAX_INPUT_FILES * sizeof(char*));
	int i;
	int index = 0;
	// for(i = 0; i < MAX_INPUT_FILES; i++){
	// 	fileNames[i] == "NULL";
	// }
	for(i = 5; i < num_files; i++){
		fileNames[index] = argv[i];
		index++;
	}

	pthread_t req_threads[num_requester];
	struct thread_arguments *requester_threads[num_requester];
	for(i = 0; i < num_requester; i++){
		requester_threads[i] = malloc(sizeof(struct thread_arguments));
		requester_threads[i]->t_files = malloc(MAX_INPUT_FILES * sizeof(char *));
		//requester_threads[i]->arr = malloc(MAX_ARRAY_SIZE * sizeof(char*));
	}


	pthread_t res_threads[num_resolver];
	struct thread_arguments *resolver_threads[num_resolver];
	for(i = 0; i < num_resolver; i++){
		resolver_threads[i] = malloc(sizeof(struct thread_arguments));
		//resolver_threads[i]->arr = malloc(MAX_ARRAY_SIZE * sizeof(char*));
	}
	int arr_idx = -1;
	bool exists = true;
	for(i = 0; i < num_requester + num_resolver; i++){
		if(i < num_resolver){
			// int z;
			// for(z = 0; z < MAX_ARRAY_SIZE; z++){
			// 	resolver_threads[i]->arr[z] = array[z];
			// }
			resolver_threads[i]->en = &e;
			resolver_threads[i]->de = &d;
			resolver_threads[i]->arr = array;
			resolver_threads[i]->front_array = &arr_idx;
			resolver_threads[i]->results = result_log;
			resolver_threads[i]->req_exist = &exists;
			resolver_threads[i]->lock = &lock;
			resolver_threads[i]->sda = &sda;
			resolver_threads[i]->service_file = &service_file;
			resolver_threads[i]->result_file = &result_file;
			resolver_threads[i]->items_avail = &items_avail;
			resolver_threads[i]->space_avail = &space_avail;
			if(pthread_create(&res_threads[i], NULL, (void *) &resolver_func, (void *) resolver_threads[i])){
				printf("Error creating thread\n");
			}
		}
		if(i < num_requester){
			int j; 
			for(j = 0; j < MAX_INPUT_FILES; j++){
				requester_threads[i]->t_files[j] = fileNames[j];
			}
			// for(j = 0; j < MAX_ARRAY_SIZE; j++){
			// 	requester_threads[i]->arr[j] = array[j];
			// }
			requester_threads[i]->en = &e;
			requester_threads[i]->de = &d;
			requester_threads[i]->arr = array;
			requester_threads[i]->array_index = i;
			requester_threads[i]->serviced = service_log;
			requester_threads[i]->num_serviced = 0;
			requester_threads[i]->num_threads = num_requester;
			requester_threads[i]->thread_num = i + 1;
			requester_threads[i]->front_array = &arr_idx;
			requester_threads[i]->lock = &lock;
			requester_threads[i]->sda = &sda;
			requester_threads[i]->service_file = &service_file;
			requester_threads[i]->result_file = &result_file;
			requester_threads[i]->items_avail = &items_avail;
			requester_threads[i]->space_avail = &space_avail;
			if(pthread_create(&req_threads[i], NULL, (void *) &requester_func, (void *) requester_threads[i])){
				printf("Error creating thread\n");
			}
		}
	}
	for(i = 0; i < num_requester; i++){
		pthread_join(req_threads[i], 0);
		printf("JOIN REQ\n");
		free(requester_threads[i]->t_files);
		// free(requester_threads[i]->arr);
		free(requester_threads[i]);
	}
	printf("FINISH REQ\n");
	exists = false;
	for(i = 0; i < num_resolver; i++){
		pthread_join(res_threads[i], 0);
		printf("RES JOINED\n");
		// free(resolver_threads[i]->arr);
		free(resolver_threads[i]);
	}
	printf("FINISHED RES\n");

	free(array);
	free(fileNames);
	fclose(service_log);
	fclose(result_log);
	gettimeofday(&end, NULL);
	printf("Total Runtime: %lf seconds\n", (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec)/1000000.0));

	return 0;
}