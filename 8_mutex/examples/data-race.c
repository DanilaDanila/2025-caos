#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>

pthread_mutex_t *mutex = NULL;
int buffer[1024] = {0,};

void *worker(void *args) {
    int *local_buffer = (int *)args;

    for (int i = 0; i < 1024; ++i) {
        pthread_mutex_lock(mutex);

        int current = local_buffer[i];
        int new = current + 1;
        local_buffer[i] = new;

        pthread_mutex_unlock(mutex);
    }
}

int main() {
    mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    pthread_t threads[2];
    
    pthread_create(threads + 0, NULL, worker, (void*)(buffer));
    pthread_create(threads + 1, NULL, worker, (void*)(buffer));
    
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_mutex_destroy(mutex);
    free(mutex);
    
    int sum = 0;
    for (int i = 0; i < 1024; ++i) {
        sum += buffer[i];
    }
    printf("sum=%d\n", sum);

    return 0;
}

