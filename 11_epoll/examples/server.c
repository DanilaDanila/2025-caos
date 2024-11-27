#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>   // perror
#include <stdlib.h>  // exit
#include <sys/epoll.h>
#include <fcntl.h>

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

    shutdown(fd, SHUT_RDWR);
    close(fd);
}


int main() {
    int epoll_fd = epoll_create1(0);

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

    int flags = fcntl(socket_fd, F_GETFD);
    flags |= O_NONBLOCK;
    fcntl(socket_fd, F_SETFD, flags);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);


    struct epoll_event events[10];
    for (;;) {
        int n = epoll_wait(epoll_fd, events, 10, -1);

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == socket_fd) {
                    int client_fd = accept(socket_fd, NULL, NULL);
                    if (client_fd < 0) {
                        perror("accept");
                        exit(6);
                    }

                    int flags = fcntl(client_fd, F_GETFD);
                    flags |= O_NONBLOCK;
                    fcntl(client_fd, F_SETFD, flags);

                    struct epoll_event ev;
                    ev.events = EPOLLIN;
                    ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
            } else {
                    process_client(events[i].data.fd);
            }
        }
    }

    // unreachable
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

