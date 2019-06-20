#ifdef __AVR__
#include "pins_arduino.h"
#endif

//Based on https://github.com/arduino/ArduinoCore-avr 2/11/2019

#define MAGIC_KEY 0x7777
#define MAGIC_KEY_POS 0x0800

typedef uint8_t byte;

#define F_CPU 16000000

#define HIGH 0x1
#define LOW  0x0

extern "C"
{
    void init();
    void setup();
    void loop();
    void randomSeed(unsigned long);
    void delay(uint32_t milliseconds);
    unsigned long millis();
    unsigned long micros();
}

long random(long howsmall, long howbig);

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))