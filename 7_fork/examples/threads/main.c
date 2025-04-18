#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include <unistd.h>


int my_global_int_value;


void *my_routine(void *args) {
    sleep(3);

    uint64_t num = (uint64_t)(args);

    uint64_t sum = 0;
    for (int i = 0; i < num; ++i) {
        sum += i;
    }

    printf("my_global_int_value = %d\n", my_global_int_value);
    fflush(stdout);

    return (void*)(sum);
}


int main() {
    my_global_int_value = 111;

    pthread_t worker;

    int success = pthread_create(
        &worker,
        NULL,
        &my_routine,
        (void*)(11)
    );

    if (success != 0) {
        perror("pthread_create");
        return 1;
    }

    printf("thread started!\n");

    my_global_int_value = 2222;

    // .... ///
    
    printf("waiting 4 thread\n");

    void *pthread_return_value;
    pthread_join(worker, &pthread_return_value);

    printf("got thread\n");

    uint64_t sum = (uint64_t)(pthread_return_value);
    printf("sum = %d\n", sum);
}

