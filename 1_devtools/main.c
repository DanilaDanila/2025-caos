#include <stdio.h>

void greet(char *name) {
    printf("Hello, %s\n", name);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage:\n\t%s name\n", argv[0]);
        return 1;
    }

    greet(argv[1]);
    return 0;
}
