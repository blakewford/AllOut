//#include "assets.h"
#include "bus.h"
#include "bike.h"
#include "truck.h"
#include "compressed.h"
#include "cube.h"

#include <Models.h>
#include <Arduino.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>

#include "stdlib.h"
#include "bitmap.h"

#include <cmath>

Models models;
Sprites sprites;
Arduboy2Base arduboy;
//ArduboyTones sound(arduboy.audio.enabled);

const int16_t START_ANGLE = 180;

uint8_t gScene = 0;
int16_t yAngle = START_ANGLE;

bool decrement = true;
const int8_t MAX_ANGLE = 30;

void setup()
{
    arduboy.begin();
    arduboy.setFrameRate(60);
    models.begin();
    arduboy.initRandomSeed();
}

int8_t count = 0;
const int8_t HoldCount = 16;

int8_t gSelection = 0;

int32_t gDebugIndex = 0;
uint8_t* gDebugFill = nullptr;
void printBitmapColor(const uint8_t* bitmap, int32_t middle, int32_t dim, int32_t offsetX, int32_t offsetY, int32_t requiredTitles)
{
    int32_t start = middle + (offsetX*4)*5 + ((dim*4)*offsetY)*5;
    int32_t skinColor = bitmap[start+3] << 16 | bitmap[start+2] << 8 | bitmap[start+1];

    int32_t ndx = -1;
    int32_t leastDiff = 0x7FFFFFFF;
    int32_t numColors = sizeof(color)/sizeof(int32_t);
    while(numColors--)
    {
        int32_t diff = std::abs(skinColor - color[numColors]);
        if(diff < leastDiff)
        {
            leastDiff = diff;
            ndx = numColors;
        }
    }

    gDebugFill[gDebugIndex] = ndx;
    gDebugIndex++;

    if(gDebugIndex == requiredTitles) gDebugIndex = 0;
}

void spiral(const uint8_t* bitmap, int32_t middle, int32_t dim, int32_t total)
{
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    int32_t tempX = 1;
    int32_t tempY = 1;

    const int32_t requiredTitles = total;

    while(total > 0)
    {
//        printf("RIGHT\n");
        offsetX++;
        tempY--;

        while(total > 0 && tempX <= offsetX)
        {
            printBitmapColor(bitmap, middle, dim, tempX, tempY, requiredTitles);
            total--;
            tempX++;
        }

//        printf("UP\n");
        tempX--;
        tempY--;
        offsetY++;
        offsetY = -offsetY;
        while(total > 0 && tempY >= offsetY)
        {
            printBitmapColor(bitmap, middle, dim, tempX, tempY, requiredTitles);
            total--;
            tempY--;
        }

//        printf("LEFT\n");

        tempX = offsetX;
        tempX--;
        offsetX++;
        offsetX = -offsetX;
        while(total > 0 && tempX > offsetX)
        {
            printBitmapColor(bitmap, middle, dim, tempX, offsetY, requiredTitles);
            total--;
            tempX--;
        }

//        printf("DOWN\n");

        tempX++;
        tempY = offsetY;
        tempY++;
        offsetY = -offsetY;
        offsetY++;
        while(total > 0 && tempY < offsetY)
        {
            printBitmapColor(bitmap, middle, dim, tempX, tempY, requiredTitles);
            total--;
            tempY++;
        }
        offsetX = -offsetX;
        offsetX--;
        offsetY--;
        tempX++;
    }
}

int8_t last = 1;
void selection()
{
    float* modelMap = nullptr;
    uint8_t* vehicle = nullptr;
    uint8_t* name = nullptr;
    uint8_t* fill = nullptr;
    switch(gSelection)
    {
        case 0:
            vehicle = cube;
            modelMap = ndxToValueCube;
            name = overland;
            fill = fillCube;
            break;
        case 1:
            vehicle = truck;
            modelMap = ndxToValueTruck;
            name = baja;
            fill = fillTruck;
            break;
        case 2:
            vehicle = bus;
            modelMap = ndxToValueBus;
            name = burningman;
            fill = fillCar;
            break;
        case 3:
            vehicle = bike;
            modelMap = ndxToValueBike;
            name = moto;
            fill = fillCar;
            break;
    }

#if 0
    const int32_t requiredTiles = modelMap[0];

    float dim = ceil(sqrt(requiredTiles)) + 2;
    ((int)dim%2) == 0 ? dim: dim++;
    dim*=5;

    FILE* skin = fopen("./tools/skin.bmp","rb");
    if(skin)
    {
        gDebugFill = new uint8_t[requiredTiles];

        fseek(skin, 0, SEEK_END);
        int32_t size = ftell(skin);
        rewind(skin);
        uint8_t* binary = (uint8_t*)malloc(size);
        size_t read = fread(binary, 1, size, skin);
        if(read != size) return -1;
        fclose(skin);

        int32_t offsetX = 0;
        int32_t offsetY = 0;
        int32_t pixelOffset = binary[10/*BEGINPXLOFFSET*/];
        const int32_t middle = ((size - pixelOffset)/2) + pixelOffset + (2*(dim*4)) + (dim*2) + 4;

        spiral(binary, middle, dim, requiredTiles);

        free(binary);
        binary = nullptr;
        fill = gDebugFill;
    }
#endif

//    models.drawModel(obj, 15, yAngle, 0, 1);
    models.drawCompressedModel(vehicle, modelMap, fill, 15, yAngle, 0);
//    sprites.drawSelfMasked(3, 16, left, 0);
//    sprites.drawSelfMasked(93, 16, right, 0);
//    sprites.drawSelfMasked(43, 56, name, 0);
    yAngle++;

    if(arduboy.justPressed(LEFT_BUTTON))
    {
//        gSelection--;
    }

    if(arduboy.justPressed(RIGHT_BUTTON))
    {
 //       gSelection++;
    }

    if(arduboy.justPressed(A_BUTTON))
    {
        gScene = 2;
    }

    if(gSelection < 0)
    {
        gSelection = 3;
    }

    if(gSelection > 3)
    {
        gSelection = 0;
    }

    if(gDebugFill != nullptr)
    {
        delete[] gDebugFill;
        gDebugFill = nullptr;
    }
}

void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();
    arduboy.clear();

    selection();

    arduboy.display();
}
