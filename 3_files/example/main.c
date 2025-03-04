#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

    struct stat sbuff;

    if (lstat(argv[1], &sbuff) != 0) {
        perror("stat");
        return 2;
    }

    switch (sbuff.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");     break;
        case S_IFCHR:  printf("character device\n"); break;
        case S_IFDIR:  printf("directory\n");        break;
        case S_IFIFO:  printf("FIFO/pipe\n");        break;
        case S_IFLNK:  printf("symlink\n");          break;
        case S_IFREG:  printf("regular file\n");     break;
        case S_IFSOCK: printf("socket\n");           break;
        default:       printf("unknown?\n");         break;
    }
    printf("TOTAL SIZE IN BYTES: %d\n", sbuff.st_size);

    return 0;
}

