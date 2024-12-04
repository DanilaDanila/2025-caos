#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>


int main(int argc, char **argv) {
    char *so_path = argv[1];

    void *dl = dlopen(
        so_path,
        RTLD_NOW | RTLD_LOCAL
    );
    if (dl == NULL) {
        perror("dlopen");
        return 1;
    }

    char func_name[] = "say_smth";

    void (*say_smth)(void) = dlsym(
        dl,
        func_name
    );
    if (say_smth == NULL) {
        perror("dlsym");
        return 2;
    }

    say_smth();

    if (dlclose(dl) != 0) {
        perror("dlclose");
        return 3;
    }
}

