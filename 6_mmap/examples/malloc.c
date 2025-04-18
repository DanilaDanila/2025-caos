#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

/* ФИЗ_ПАМЯТЬ --> ВИРТ_ПАМЯТЬ
 * ФИЗ_ПАМЯТЬ:
 *  - RAM
 *  - HDD / SSD / NVME
 *  - магнитная лента
 *  - дискета
 *  - любое блочное устройство
 *    (чисто технически)
 *
 *  файл
 */

int main() {
    // C: malloc(...) / free(...)
    // C++: new / delete

    // 1Kb (RAM)
    char *buffer = mmap(
        NULL,
        1024 * sizeof(char),
        PROT_WRITE | PROT_READ,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if (buffer == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    for (int i = 0; i < 10; ++i) {
        buffer[i] = (char)i;
    }

    for (int i = 0; i < 10; ++i) {
        printf("buffer[%d] = %d\n", i + 5, buffer[i + 5]);
    }

    munmap(buffer, 1024 * sizeof(char));

    return 0;
}

