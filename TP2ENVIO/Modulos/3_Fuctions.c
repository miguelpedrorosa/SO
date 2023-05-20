#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int *vector;
    int max_value;
    int start;
    int end;

}ThreadInformation;

void *find_max_value(void *arg){
    ThreadInformation *Data = (ThreadInformation*) arg;
    Data->max_value = Data->vector[Data->start];

    for(int i=Data->start + 1 ; i< Data->end ; ++i){
        if(Data->vector[i]>Data->max_value){
            Data->max_value = Data->vector[i];
        }
    }

    pthread_exit(NULL);

}

void create_threads(pthread_t* threads, ThreadInformation* thread_data, int num_threads,int* vector, int size){
    int thread_vector_size = size / num_threads;
    int start = 0;
    int end = 0;

    for(int i = 0; i< num_threads; ++i){
        start=end;
        end=start + thread_vector_size;

        thread_data[i].vector = vector;
        thread_data[i].start = start;
        thread_data[i].end = end;

        pthread_create(&threads[i], NULL, find_max_value, (void*)&thread_data[i]);
    }

}

int find_max_vector_value(int *vector,int size,int num_threads){
    pthread_t threads[num_threads];
    ThreadInformation thread_data[num_threads];
     
    create_threads(threads,thread_data,num_threads,vector,size);

    int max_value = vector[0];

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
        if (thread_data[i].max_value > max_value) {
            max_value = thread_data[i].max_value;
        }
    }

    return max_value;

}