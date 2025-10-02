#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>


int main() {
    struct stat buf;

    // int stat_result = stat("symlink_to_readme.txt", &buf);
    int stat_result = lstat("symlink_to_readme.txt", &buf);
    if (stat_result == -1) {
        perror("cannot stat file");
        return 1;
    }

    if ((buf.st_mode & S_IFMT) == S_IFREG) {
        printf("regular file\n");
    } else if ((buf.st_mode & S_IFMT) == S_IFDIR) {
        printf("directory\n");
    } else if ((buf.st_mode & S_IFMT) == S_IFLNK) {
        printf("link\n");
    } else {
        printf("smth else\n");
    }
}

