#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

typedef struct
{
    Matrix *matrix;
    int rowIndex;
    int sum;
} ThreadInformation;

void *row_sum(void *args)
{
    ThreadInformation *thread_Data = (ThreadInformation *)args;
    Matrix *matrix = thread_Data->matrix;

    int rowIndex = thread_Data->rowIndex;
    int sum = 0;

    for (int i = 0; i < matrix->nColumns; ++i)
    {
        sum += matrix->values[rowIndex][i];
    }

    thread_Data->sum = sum;

    pthread_exit(NULL);
}

void row_matrix_sum(Matrix *matrix)
{
    int nRows = matrix->nRows;

    pthread_t *threads = malloc(nRows * sizeof(pthread_t));
    ThreadInformation *thread_Data = malloc(nRows * sizeof(ThreadInformation));

    for (int i = 0; i < nRows; ++i)
    {
        thread_Data[i].matrix = matrix;
        thread_Data[i].rowIndex = i;
        pthread_create(&threads[i], NULL, row_sum, &thread_Data[i]);
    }

    for (int i = 0; i < nRows; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    for(int i=0 ; i<nRows ; ++i){
        printf("Soma da linha %d: %d\n",i+1,thread_Data[i].sum);
    }

    free(thread_Data);
    free(threads);
}