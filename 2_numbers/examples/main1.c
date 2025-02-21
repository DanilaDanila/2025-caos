#include <stdio.h>

int main() {
    struct {
        char is_negative;  // 0, 1
        unsigned int x;
    } signed_x;
    
    // sizeof(x) = 4B;
    // sizeof(signed_x) = 4B + 1 bit = 5B

    typedef struct {
        char is_negative : 1; // 0, 1
        unsigned int x : 31;
    } sint;

    // sizeof(int) 
    // = sizefof(sint)

    sint s_add(sint x, sint y) {
        if (x.is_negative == y.is_negative) {
            // знак один
        } else {
            // знак разный
        }
    }

    // 1  = 0b00000001
    // -1 = 0b10000001
    //
    // +0 = 0b00000000
    // -0 = 0b10000000



    return 0;
}

