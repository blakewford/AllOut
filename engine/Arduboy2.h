#pragma once

#include <stdint.h>

#include "Sprites.h"
#include "Arduboy2Audio.h"

struct Rect
{
    int16_t x;
    int16_t y;
    uint8_t width;
    uint8_t height;
};

#define WIDTH 640
#define HEIGHT 480

class Arduboy2Base
{
public:
        void begin();
        void setFrameRate(uint8_t rate);
        void initRandomSeed();
        bool everyXFrames(uint8_t frames);
        bool justPressed(uint8_t button);
        bool collide(Rect rect1, Rect rect2);
        bool nextFrame();
        void pollButtons();
        void clear();
        void display();
static  void drawPixel(int16_t x, int16_t y, uint8_t color = 1);
        void drawFastHLine(int16_t x, int16_t y, uint16_t w, uint8_t color, uintptr_t texture);
        void fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color, uintptr_t texture);

        Arduboy2Audio audio;
};
