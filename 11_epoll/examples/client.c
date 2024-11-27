#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>   // perror
#include <stdlib.h>  // exit
#include <sys/wait.h>


void write_all(int fd, const char *msg, size_t size) {
    size_t total = 0;
    while (total < size) {
        total += write(
            fd,
            msg + total,
            size - total
        );
    }
}


void read_all(int fd, char *buffer, size_t size) {
    read(fd, buffer, size);
    // size_t total = 0;
    // for (;;) {
    //     int current = read(
    //         fd,
    //         buffer + total,
    //         size - total
    //     );

    //     if (current == 0) {
    //         break;
    //     }
    //     total += current;
    // }
}


void run_once() {
    const char msg_to_server[] = "hello, server";

    int socket_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );
    if (socket_fd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(6565),
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
    };

    if (
        connect(
            socket_fd,
            (struct sockaddr*)(&addr),
            sizeof(addr)
        ) < 0
    ) {
        perror("connect");
        exit(2);
    }

    sleep(1);
    write_all(socket_fd, msg_to_server, sizeof(msg_to_server));

    char buffer[128];
    read_all(socket_fd, buffer, sizeof(buffer));

    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);

    dprintf(2, "communication done!\n");
}

void routine(int n_times) {
    for (int i = 0; i < n_times; ++i) {
        run_once();
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf(
            "%s <n_times> <n_threads>\n",
            argv[0]
        );
        return 1;
    }

    int n_times = atoi(argv[1]);
    int n_threads = atoi(argv[2]);

    if (n_times <= 0 || n_threads <= 0) {
        printf("<n_times> and <n_times> must be positive");
        return 2;
    }

    for (int i = 1; i < n_threads; ++i) {
        switch (fork()) {
            case -1:
                perror("fork");
                exit(3);
            case 0:
                routine(n_times);
                exit(0);
            default:
                break;
        }
    }
    routine(n_times);

    for (int i = 1; i < n_threads; ++i) {
        wait(NULL);
    }
}

