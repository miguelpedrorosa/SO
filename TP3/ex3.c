#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} countdown_t;

int countdown_init(countdown_t *cd, int initialValue)
{
    cd->value = initialValue;
    if (cd->value < 0)
    {
        perror("valor inicial inválido");
        return -1;
    }

    if (pthread_mutex_init(&cd->mutex, NULL) != 0)
    {
        perror("erro a iniciar o mutex");
        return -1;
    }

    if (pthread_cond_init(&cd->condition, NULL) != 0)
    {
        perror("erro a iniciar o cond");
        pthread_mutex_destroy(&cd->mutex);
        return -1;
    }

    return 0;
}

int countdown_destroy(countdown_t *cd)
{
    if (pthread_mutex_destroy(&cd->mutex) != 0)
    {
        perror("erro a destruir o mutex");
        return -1;
    }

    if (pthread_cond_destroy(&cd->condition) != 0)
    {
        perror("erro a destruir o cond");
        return -1;
    }

    return 0;
}

int countdown_wait(countdown_t *cd)
{
    if (pthread_mutex_lock(&cd->mutex) != 0)
    {
        perror("erro no mutex lock do wait");
        return -1;
    }

    while (cd->value > 0)
    {
        if (pthread_cond_wait(&cd->condition, &cd->mutex) != 0)
        {
            perror("erro no cond_wait");
            pthread_mutex_unlock(&cd->mutex);
            return -1;
        }
    }

    if (pthread_mutex_unlock(&cd->mutex) != 0)
    {
        perror("erro no mutex_unlock do wait");
        return -1;
    }

    return 0;
}

int countdown_down(countdown_t *cd)
{
    if (pthread_mutex_lock(&cd->mutex) != 0)
    {
        perror("erro no mutex_lock do down");
        return -1;
    }

    cd->value--;
    if (cd->value == 0)
    {
        if (pthread_cond_broadcast(&cd->condition) != 0)
        {
            perror("erro no cond_broadcast");
            pthread_mutex_unlock(&cd->mutex);
            return -1;
        }
    }

    if (pthread_mutex_unlock(&cd->mutex) != 0)
    {
        perror("erro no mutex_unlock do wait");
        return -1;
    }

    return 0;
}

void *countdown_thread(void *arg)
{
    countdown_t *cd = (countdown_t *)arg;

    printf("Threads starting...\n");

    // Simular tempos diferentes para cada thread
    sleep(rand() % 5 + 1);

    countdown_down(cd);
    printf("Thread %d is waiting.\n", cd->value);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[1]);

    countdown_t cd;
    countdown_init(&cd, nthreads);

    pthread_t threads[nthreads];

    for (int i = 0; i < nthreads; i++)
    {
        pthread_create(&threads[i], NULL, countdown_thread, (void *)&cd);
    }

    countdown_wait(&cd);


    for (int i = 0; i < nthreads; i++)
    {
        pthread_join(threads[i], NULL);
        printf("All threads are done\n");
    }

    countdown_destroy(&cd);

    return 0;
}