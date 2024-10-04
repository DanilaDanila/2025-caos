#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv, char **env) {
    
    int *data = malloc(sizeof(int) * 1024);

    data[0] = 123;

    free(data);

    printf("%d\n", data[0]);

    return 0;
}



