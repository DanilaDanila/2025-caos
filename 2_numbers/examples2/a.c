#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


// wchar_t

// a4 d0  --> LE (d0a4) | BE (a4d0)
// 9f d0 
// 9c d0
// ....


int main() {
    wchar_t wstr[] =
        {'Ф', 'П', 'М', 'И', '\0'};

    for (
        wchar_t *it = wstr;
        *it != '\0';
        ++ it
    ) {
        printf("%x\n", *it);
    }
}

