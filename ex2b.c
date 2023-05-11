#include <stdio.h>
#include <pthread.h>

#define MAX_TAREFAS 10

void *print_numero(void *arg) {
    int numero = *(int *)arg;
    printf("%d\n", numero);
    return NULL;
}

int main() {
    pthread_t tarefas[MAX_TAREFAS];
    int numeros[MAX_TAREFAS];
    int teste;

    for (int i = 0; i < MAX_TAREFAS; i++){
        numeros[i] = i;
        teste = pthread_create(&tarefas[i], NULL, print_numero, &numeros[i]);
        if (teste != 0){
            printf("Erro ao criar a tarefa %d.\n", i);
            perror(NULL);
            return -1;
        }
    }

    for (int i = 0; i < MAX_TAREFAS; i++){
        teste = pthread_join(tarefas[i], NULL);
        if (teste != 0){
            printf("Erro ao aguardar a conclusão da tarefa %d.\n", i);
            perror(NULL);
            return -1;
        }
    }

    printf("Todas as tarefas concluídas com sucesso\n");

    return 0;
}