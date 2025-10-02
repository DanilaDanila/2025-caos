#include <stdio.h>

enum {
    OS_LINUX = 0,
    OS_MACOS = 1,
    OS_WINDOWS = 2
};

#ifdef __linux__
"I AM LINUXX!"
#else
123
#endif

int main() {
    if (MY_CURRENT_OS == OS_LINUX) {
        printf("hello, linux\n");
    } else {
        printf("SWITCH TO LINUX\n");
    }
}



x86_32
x86_64

ARM

RISC-V

C / Fortran / BASIC / PASCAL / Rust

5_языков x 5_архитектур = 25 компиляторов


clang
llvm

C       ->
Fortran -> (C--)         -> x86_64
BASIC   -> LLVM ASSEMBLY -> ARM
PASCAL  ->   LANGUAGE    -> RISC-V
Rust    ->





