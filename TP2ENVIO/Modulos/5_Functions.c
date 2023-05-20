#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "matrix.h"

typedef struct
{
    Matrix *m1;
    Matrix *m2;
    Matrix *result;
} ThreadInformation;

void *matrix_multiplication_thread(void *args)
{
    ThreadInformation *thread_Data = (ThreadInformation *)args;

    thread_Data->result = matrix_multiplication(thread_Data->m1, thread_Data->m2);

    pthread_exit(NULL);
}

Matrix *matrix_multiplication_with_threads(Matrix *m1, Matrix *m2, int nThreads)
{
    if (m1->nColumns != m2->nRows)
    {
        return NULL;
    }

    Matrix *result = matrix_create(m1->nRows, m2->nColumns);
    pthread_t *threads = malloc(nThreads * sizeof(pthread_t));
    ThreadInformation *Thread_Data = malloc(nThreads * sizeof(ThreadInformation));

    int nlines = m1->nRows / nThreads;
    int initline = 0;

    for (int i = 0; i < nThreads; ++i)
    {
        initline = i * nlines;

        Thread_Data[i].m1 = matrix_get_sub(m1, initline, nlines);
        Thread_Data[i].m2 = m2;

        pthread_create(&threads[i], NULL, matrix_multiplication_thread, &Thread_Data[i]);
    }

    for (int i = 0; i < nThreads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < nThreads; ++i)
    {
        initline = i * nlines;

        matrix_put_sub(result, Thread_Data[i].result, initline, nlines);
    }

    free(threads);
    free(Thread_Data);

    return result;
}