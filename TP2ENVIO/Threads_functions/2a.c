#include <stdio.h>
#include <pthread.h>

#define NUM_MAX 80

void *escrever_zero(void *arg) {
    for (int i = 0; i < NUM_MAX; i++) {
        printf("0\n");
    }
    return NULL;
}

int main() {
    pthread_t tarefa;
    int teste;

    teste = pthread_create(&tarefa, NULL, escrever_zero, NULL);
    if (teste != 0){
        perror("Erro ao criar a tarefa.\n");
        return 1;
    }

    teste = pthread_join(tarefa, NULL);
    if (teste != 0){
        perror("Erro ao aguardar a conclusão da tarefa.\n");
        return -1;
    }

    printf("Tarefa concluída com sucesso.\n");

    return 0;
}