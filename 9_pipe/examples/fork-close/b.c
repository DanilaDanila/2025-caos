#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int arc, char **argv) {
    int fd = open(argv[1], O_RDONLY);
    printf("fd = %d\n", fd);

    for (;;) {}
}
