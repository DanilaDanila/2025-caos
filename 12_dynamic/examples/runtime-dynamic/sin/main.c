#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>


int main(int argc, char **argv) {
    /* если передать абсолютный путь без
     * точки, то dlopen работает
     * а dlsym - нет
     * :cry:
     */
    char *so_path = argv[1];

    void *dl = dlopen(
        so_path,
        RTLD_NOW | RTLD_LOCAL
    );
    if (dl == NULL) {
        perror("dlopen");
        return 1;
    }

    char func_name[] = "my_sin";

    void *my_sin_ptr = dlsym(
        dl,
        func_name
    );
    if (my_sin_ptr == NULL) {
        perror("dlsym");
        return 2;
    }

    float (*dl_sin)(float) = my_sin_ptr;

    float x = 3.1415926 / 6.0;
    float sin_x = dl_sin(x);

    if (dlclose(dl) != 0) {
        perror("dlclose");
        return 3;
    }

    printf(
        "sin(%f) = %f",
        x, sin_x
    );
}

