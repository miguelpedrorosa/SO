#include <pthread.h>

typedef struct
{
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} countdown_t;

int countdown_init(countdown_t *cd, int initialValue)
{
    cd->value = initialValue;
    pthread_mutex_init(&cd->mutex, NULL);
    pthread_cond_init(&cd->mutex, NULL);
}


int countdown_destroy(countdown_t *cd)
{
    pthread_mutex_destroy(&cd->mutex);
    pthread_cond_destroy(&cd->condition);
    return 0;
}


int countdown_wait (countdown_t *cd)
{
    pthread_mutex_lock(&cd->mutex);
    while (cd->value > 0)
    {
        pthread_cond_wait(&cd->condition, &cd->mutex);
    }
    pthread_mutex_unlock(&cd->mutex);
    return 0;  
}


int countdown_down(countdown_t *cd)
{
    pthread_mutex_lock(&cd->mutex);
    cd->value--;
    if (cd->value == 0)
    {
        pthread_cond_broadcast(&cd->condition);
    }
    pthread_mutex_unlock(&cd->mutex);
    return 0;
}