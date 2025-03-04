#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

const int count = 1024;

int main(int argc, char **argv) {
    /* 0 - стандартный поток ввода - stdin
     * 1 - стандартный поток вывода - stdout
     * 2 - стандартный поток ошибок - stderr
     */

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY, 0);
    if (fd == -1) {
        perror("open");
        return 2;
    }
    printf("fd = %d\n", fd);

    char buffer[count];
    int len;
    for (;;) {
        len = read(fd, buffer, count);

        if (len <= 0) {
            break;
        }

        write(1, buffer, len);
    }

    close(fd);
    return 0;
}

