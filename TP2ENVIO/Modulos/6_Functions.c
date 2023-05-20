#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "matrix.h"
#include "6_Functions.h"

#define MAX_BUFFER_SIZE 1024




void send_matrix(int socket, const Matrix *matrix ){
    write(socket, &(matrix->nRows), sizeof(int));
    write(socket, &(matrix->nColumns), sizeof(int));
    for (int i = 0; i < matrix->nRows; i++) {
        write(socket, matrix->values[i], matrix->nColumns * sizeof(int));
    }

}

Matrix *receive_matrix(int socket){
     Matrix *matrix = (Matrix *)malloc(sizeof(Matrix));
    read(socket, &(matrix->nRows), sizeof(int));
    read(socket, &(matrix->nColumns), sizeof(int));

    matrix->values = (int **)malloc(matrix->nRows * sizeof(int *));
    for (int i = 0; i < matrix->nRows; i++) {
        matrix->values[i] = (int *)malloc(matrix->nColumns * sizeof(int));
        read(socket, matrix->values[i], matrix->nColumns * sizeof(int));
    }

    return matrix;
}

void *handle_client_Mul(void *args){
    ThreadInformation *thread_Data = (ThreadInformation*) args;

    int clientSocket = thread_Data->client_socket;
    Matrix *m1 = thread_Data->m1;
    Matrix *m2 = thread_Data ->m2;

    Matrix *result = matrix_multiplication(m1,m2);

    send_matrix(clientSocket,result);

    matrix_destroy(result);
    close(clientSocket);

    
    free(thread_Data);

    pthread_exit(NULL);
}