#include <stdint.h>
#include <string.h>

typedef bool boolean;

#define B00000000  0x0
#define B00000001  0x1
#define B00000010  0x2
#define B00000100  0x4
#define B00001000  0x8
#define B00001010  0xA
#define B00100000 0x20

enum: int8_t
{
    LEFT_BUTTON,
    RIGHT_BUTTON,
    UP_BUTTON,
    DOWN_BUTTON,
    A_BUTTON,
    B_BUTTON,
};

char* ltoa(long l, char * buffer, int radix);

#define pgm_read_word
