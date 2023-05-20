#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "3_Fuctions.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    if (argc < 3)
    {
        printf("Uso: ./programa tamanho_vetor numero_tarefas\n");
        return -1;
    }

    int size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    int *vector = malloc(size * sizeof(int));
    for (int i = 0; i < size; ++i)
    {
        vector[i] =  rand()%100;
    }

    int max_value = find_max_vector_value(vector, size, num_threads);

    printf("Maior valor: %d\n",max_value);

    free(vector);

    return 0;
}
