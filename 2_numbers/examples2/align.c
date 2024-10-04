#include <stdio.h>


// 0x0 0x1 0x2 0x3 ....
//


// uint32_t b;
// uint64_t c = *((uint64_t*)(&b))


// uint64_t a;
//      assert(
//          &(a) % (sizeof(a)) == 0
//      )



int main() {
    // aligned
    typedef struct {
        uint32_t number;
        // padding 4byte
        int64_t large_number;
        char /*int8_t*/ simbol;
        // padding 7byte
    } S;

    S s;

    s.number; // &s + __offset__(s, number) = &s + 0

    s.large_number;  // &s + __offset__(s, large_number) = &s + 4byte + i(4 byte)
    
    S arr_s[10];

    arr_s[1].number; // &arr_s[1] + __offset__(s, number) = arr_s + 17byte


    typedef struct {
        int64_t large_number;
        uint32_t number;
        char /*int8_t*/ simbol;
    } __attribute__((packed));


    struct ELF {
        header h;
    } 
}




