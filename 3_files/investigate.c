#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("root_property/HARDEST_CAOS_TASK_EVER", O_RDONLY);

    if (fd < 0) {
        char msg[] = "smth fails\n";
        write(2, msg, sizeof(msg));
        _exit(1);
    }

    char buffer[2048];
    int len = read(fd, buffer, 2048);

    write(1, buffer, len);
    return 0;
}

