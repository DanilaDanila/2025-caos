
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char **argv) {
    int pipe_fd1 = open(argv[1], O_RDONLY);
    int pipe_fd2 = open(argv[2], O_RDONLY);

    int flags = fcntl(pipe_fd1, F_GETFD);
    flags |= O_NONBLOCK;
    fcntl(pipe_fd1, F_SETFD, flags);

    flags = fcntl(pipe_fd2, F_GETFD);
    flags |= O_NONBLOCK;
    fcntl(pipe_fd2, F_SETFD, flags);

    int epoll_fd = epoll_create1(0);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = pipe_fd1;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd1, &ev) == -1) {
        perror("epoll add 1");
        return 1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = pipe_fd2;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd2, &ev) == -1) {
        perror("epoll add 2");
        return 1;
    }


    struct epoll_event events[1];
    for (;;) {
        int n = epoll_wait(epoll_fd, events, 1, -1);

        char buffer[128];
        int len = read(events[0].data.fd, buffer, sizeof(buffer));
        write(1, buffer, len);
    }

    // unreachable
    close(epoll_fd);
    close(pipe_fd1);
    close(pipe_fd2);
}

