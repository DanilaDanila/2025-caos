#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("main.c", O_RDONLY);
    if (fd < 0) {
        perror("smth wrong with `main.c`");
        return 1;
    }

    struct stat stat_buf;
    if (stat("main.c", &stat_buf) != 0) {
        perror("smth wrong with stat");
        return 2;
    }

    size_t file_size = stat_buf.st_size;

    // void* --> char*
    // void* --> struct png*
    char *filedata = (char*)mmap(
        NULL,
        file_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );
    close(fd);

    if (filedata == MAP_FAILED) {
        perror("smth wrong with mmap");
        return 3;
    }

    for (size_t i = 0; i < file_size; ++i) {
        printf("%c", filedata[i]);
    }

    munmap(filedata, file_size);
}

