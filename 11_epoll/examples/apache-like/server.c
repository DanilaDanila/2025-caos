#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>   // perror
#include <stdlib.h>  // exit

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


void process_client(int fd) {
    const char msg_to_client[] = "hello, client";

    char buffer[128];
    read_all(fd, buffer, sizeof(buffer));

    write_all(fd, msg_to_client, sizeof(msg_to_client));

    read_all(fd, buffer, sizeof(buffer));

    shutdown(fd, SHUT_RDWR);
    close(fd);
}


int main() {
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
        .sin_addr.s_addr = INADDR_ANY,
    };

    setsockopt(
        socket_fd,
        SOL_SOCKET,
        SO_REUSEPORT | SO_REUSEADDR,
        &(int){1},
        sizeof(int)
    );

    if(
        bind(
            socket_fd,
            (struct sockaddr*)(&addr),
            sizeof(addr)
        ) < 0
    ) {
        perror("bind");
        exit(5);
    }

    if (
        listen(socket_fd, SOMAXCONN) < 0
    ) {
        perror("listen");
        exit(7);
    }

    for (;;) {
        int client_fd = accept(socket_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            exit(6);
        }

        switch(fork()) {
            case -1:
                perror("fork");
                exit(1);
            case 0:
                process_client(client_fd);
                exit(0);
            deafault:
                break;
        }
    }

    // unreachable
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

