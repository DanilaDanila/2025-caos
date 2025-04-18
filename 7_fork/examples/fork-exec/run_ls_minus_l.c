#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>


int global_value;

int *shared_value;


int main(int argc, char **argv) {
    if (argc != 2) {
        dprintf(
            STDERR_FILENO,
            "Usage: %s <path_to_dir>\n",
            argv[0]
        );
        return 1;
    }

    global_value = 111;

    const char *path_to_dir = argv[1];

    shared_value = (int*)mmap(
        NULL,
        sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        // MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (shared_value == MAP_FAILED) {
        perror("mmap");
        return 22;
    }

    *shared_value = 111;

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 2;
    }

    if (pid == 0) {
        // child process

        sleep(3);

        printf("global_value = %d\n", global_value);
        printf("*shared_value = %d\n", *shared_value);
        fflush(stdout);

        // execl("/usr/bin/ls", "ls", "-l", path_to_dir, NULL);
        execlp("echo", "echo", "hello 123");
        perror("exec");
        return 3;
    } else {
        // parent process

        global_value = 22222;
        *shared_value = 22222;

        int status;

        waitpid(pid, &status, 0);

        printf(
            "\n---\n"
            "`ls -l %s` exit status: %d"
            "\n---\n",
            path_to_dir,
            WEXITSTATUS(status)
        );
    }

    munmap(shared_value, sizeof(int));
}

