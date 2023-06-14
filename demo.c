#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "spinlock_barrier.h"

#define NUM_THREADS 4

typedef struct {
    spinlock_barrier_t * barrier;
} thread_params_t;

void* thread_do(void * arg)
{
    thread_params_t *params = (thread_params_t*) arg;
    printf("Waiting for barrier!\n");
    spinlock_barrier_wait(params->barrier);
    printf("Barrier passed!\n");
}
int main() {

    thread_params_t * params = malloc(sizeof(thread_params_t));
    params->barrier = calloc(1,sizeof(spinlock_barrier_t));
    spinlock_barrier_init(params->barrier, NUM_THREADS + 1);

    pthread_t *pthreads = malloc(NUM_THREADS * sizeof(pthread_t));

    for (size_t thread_id = 0; thread_id < NUM_THREADS; thread_id++) 
    {
        pthread_create(&pthreads[thread_id], NULL, &thread_do, (void*) params);
    }

    printf("Waiting for barrier!\n");
    spinlock_barrier_wait(params->barrier);
    printf("Barrier passed!\n");
    while(1);
    free(params);
}