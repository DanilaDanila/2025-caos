#include <stdio.h>
#include <unistd.h>


int main() {
    close(0);

    char buffer[128];
    int len = read(0, buffer, 128);

    printf("len = %d\n", len);
}
