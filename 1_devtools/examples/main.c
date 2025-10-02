

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    // int *arr = malloc(
    //     100 * sizeof(int)
    // );

    int *x = malloc(sizeof(int));
    int y = *x * 100;


    // new int[100]

    // // ./a.out DANILA
    // if (argc != 2) {
    //     printf(
    //         "Usage:\n"
    //         "\t%s <name>\n",
    //         argv[0]
    //     );
    //     return 1;
    // }

    // printf("Hello, %s\n", argv[1]);
}


// Сборка
// 1. Препроцессор  (C --> C)
// 2. Компиляция  (C --> asm*)
//  ассемблер - программа
//  язык ассемблера - мнемоники + макросы для конкретной...
// 3. Ассемблинг (asm --> operation codes)
// 4. Линковка (1.o, 2.o ... *.o + dynlinks + 
//    + elf header
//    + defaults ...
//    + entrypoint ...
//    + ...
//    --> a.out
//    )




