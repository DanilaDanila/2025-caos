#include <stdio.h>
#include <stdint.h>


int main() {
    char msg[] = "Hello, C!\n";

    // A --> 0x41
    // B --> 0x42
    // ...

    printf(msg);

    char alfabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (
        int i = 0;
        i < sizeof(alfabet) / sizeof(char);
        ++i
    ) {
        printf(
            "%c ~ 0x%x ~ %d\n",
            alfabet[i],
            alfabet[i],
            alfabet[i]
        );
    }

    printf(
        "nextof(%c) = %c\n",
        'G',
        'G' + 1
    );


    // KOI8
    // extended ASCII
    // а давайте заполним оставшуюся половину таблички
    // так, чтоб НАМ было удобно

    // KOI8 для кирилицы
    // eASCII для европейских латиниц
    // русско - норвежский словарь

    // 256 - это мало


    // UTF-16 / UNICODE
    // 1 знак - 2 байта (~2^16 комбинаций)

    // 2^10 ~ 1024 > 1000
    // 2^6 = 32
    // +- > 32000
    // 64000
    // 40000

    // IBM MAINFRAME 
    //  BIG ENDIAN
    // 
    // IBM 5100 (или 5110)
    //  LITTLE ENDIAN
    //
    // Ы - 0x102f
    //  MAINFRAME --> 0x10 0x2f
    //  LITTLE ENDIAN <-- 0x10 0x2f
    // ?? - 0x2f10

    // union {
    //     uint16_t wbukva;
    //     uint8_t bytes[2];
    // } znak;

    /* WRITE                READ
     * BIG ENDIAN           BIG ENDIAN
     * BIG ENDIAN           LITTLE ENDIAN
     * LITTLE ...
     * ...
     */

    // UTF-16
    // 0xff 0xfe  - BIG ENDIAN
    // 0xfe 0xff  - LITTLE ENDIAN
    // 0b1111110 --> dump to memory
    
    /* wchar arch = *data;
     *
     * if (arch == 0xfffe) {
     *   // same
     *   is_reverse_on_read_write = 0;
     * } else {
     *   is_reverse_on_read_write = 1;
     * }
     *
     *
     */

    // UTF-8
    // 1 знак - 1, 2, 3, 4 байта

    // в ASCII / KOI8 / eASCII / UTF-16
    // мы читали по знаку (по 1 или по 2 байта)
    //
    // то в utf-8 мы читаем по октет (по байтам)
    // 0b0??????? -- 1 байт -- ASCII-compatible
    // 0b110????? -- 2 байта
    //  - 0b10?????? - второй байт
    // 0b1110???? -- 3 байта
    //  - 0b10?????? - второй байт
    //  - 0b10?????? - третий байт
    // 0b11110??? -- 4 байта
    //  - 0b10?????? - второй байт
    //  - 0b10?????? - третий байт
    //  - 0b10?????? - четвёртый байт

    void print_utf8_binary(uint8_t u8) {
        char fstr[] = "0b00000000";

        int bit_position = 2;
        for (
            uint8_t bit_mask = 0b10000000;
            bit_mask != 0;
            bit_mask >>= 1
        ) {
            if (u8 & bit_mask) {
                fstr[bit_position++] = '1';
            }
        }

        printf(fstr);
    }

    uint8_t ya[] = "я";

    for (
        int i = 0;
        i < sizeof(ya) / sizeof(uint8_t);
        ++i
    ) {
        print_utf8_binary(ya[i]);
        printf(" ");
    }
    printf("\n");

    return 0;
}

