#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

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


struct MyData {
    uint32_t a;
    uint32_t b;
    uint32_t c;
    /* padding 16bit */
};

int main() {
    struct stat st;
    stat("data.data", &st);

    size_t file_size = st.st_size;

    int fd = open("data.data", O_RDWR, 0);

    // 1Kb (RAM)
    struct MyData *data = (struct MyData*)mmap(
        NULL,
        file_size,
        PROT_WRITE | PROT_READ,
        MAP_PRIVATE,
        fd,
        0
    );
    close(fd);


    printf(
        "MyData {\n"
        "  a = 0x%x,\n"
        "  b = 0x%x,\n"
        "  c = 0x%x,\n"
        "}\n",
        data->a,
        data->b,
        data->c
    );


    munmap(data, file_size);

    return 0;
}

