#include <stdio.h>

int main() {
    char c = -127;
    printf("-127 = %d\n", c);

    printf("-128 = %d\n", (char)(-128));
    printf("-129 = %d\n", (char)(-129));
    printf("127 = %d\n", (char)(127));
    printf("128 = %d\n", (char)(128));
    return 0;
}

