#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int count = 0;
int arr[1000];

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *factorize(void *args) {
    uint64_t n = (uint64_t)(args);

    for (int i = 2; i < n; ++i) {
        // int is_prime = 1;
        // for (int j = 2; j < i; ++j) {
        //     if (i % j == 0) {
        //         is_prime = 0;
        //         break;
        //     }
        // }

        // if (is_prime == 0) {
        //     continue;
        // }

        if (n % i == 0) {
            pthread_mutex_lock(&mutex);
            arr[count] = i;
            ++count;

            sleep(1);

            pthread_mutex_unlock(&mutex);
            pthread_cond_broadcast(&cond);
        }
    }
}


void *print_all_factors(void *) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        for (int i = 0; i < count; ++i) {
            printf("arr[%d] = %d\n", i, arr[i]);
            fflush(stdout);
        }

        count = 0;

        pthread_mutex_unlock(&mutex);
    }
}


int main() {
    pthread_t thread[3];

    pthread_create(
        &thread[0],
        NULL,
        factorize,
        (void*)(2 * 3 * 7 /* * 13 * 23 */)
    );

    pthread_create(
        &thread[1],
        NULL,
        factorize,
        (void*)(3 * 5 * 17 /* * 19 * 29 */)
    );

    pthread_create(
        &thread[2],
        NULL,
        print_all_factors,
        NULL
    );

    // pthread_create(
    //     &thread[3],
    //     NULL,
    //     print_all_factors,
    //     NULL
    // );

    for (
        int i = 0;
        i < sizeof(thread) / sizeof(pthread_t);
        ++i)
    {
        pthread_join(thread[i], NULL);
    }
}

