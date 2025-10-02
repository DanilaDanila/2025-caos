#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>


enum {
    BUFFER_SIZE = 128
};


int main() {
    /* fd = 0;  // stdin   | std::cin
     * fd = 1;  // stdout  | std::cout
     * fd = 2;  // stderr  | std::cerr
     */

    int fd = open("readme.txt", O_RDONLY, 0);

    if (fd == -1) {
        perror("cannot open file");
        return 1;
    }

    // fd = 3
    printf("fd = %d\n", fd);

    char buffer[BUFFER_SIZE];

    for (;;) {
        int len = read(fd, buffer, BUFFER_SIZE);
        if (len == -1) {
            perror("cannot read from file");
            return 2;
        }

        if (len == 0) {
            break;
        }

        int wlen = write(STDOUT_FILENO, buffer, BUFFER_SIZE);
        if (wlen == -1) {
            perror("cannot write to file");
            return 3;
        }
    }

    if (close(fd) == -1) {
        perror("cannot close file");
        return 4;
    }
}

