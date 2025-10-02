#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


/* printf(...)
 * std::cout << ....
 *
 * perror(...)
 * std::cerr << ...
 *
 *
 * 0 - STDIN_FILENO / scanf / std::cin
 * 1 - STDOUT_FILENO / printf / std::cout
 * 2 - STDERR_FILENO / perror / std::cerr
 */


enum {
    BUFFER_SIZE = 128,
};


int main() {
    int fd = open(
        "README.md",
        O_RDONLY,
        0
    );

    if (fd == -1) {
        // ошибка открытия
        perror(
            "w87oeDA< open('README.md') failed"
        );
        return 1;
    }

    char buffer[BUFFER_SIZE];
    for (;;) {
        int len = read(
            fd,
            buffer,
            BUFFER_SIZE
        );

        if (len == 0) {
            break;
        }

        if (len == -1) {
            perror("read error");
            return 3;
        }

        int wlen = write(
            STDOUT_FILENO,
            buffer,
            len
        );

        if (wlen == -1) {
            perror("write error");
            return 4;
        }
    }

    if (close(fd) == -1) {
        perror("failed to close fd");
        return 2;
    }

    return 0;
}

