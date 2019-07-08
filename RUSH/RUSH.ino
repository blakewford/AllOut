//#include "assets.h"
#include "bus.h"
#include "bike.h"
#include "truck.h"
#include "compressed.h"

#include <Models.h>
#include <Arduino.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>

#include "stdlib.h"
#include "bitmap.h"

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
            vehicle = car;
            modelMap = ndxToValueCar;
            name = overland;
            fill = fillCar;
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

//    models.drawModel(obj, 15, yAngle, 0, 1);
    models.drawCompressedModel(vehicle, modelMap, fill, 15, yAngle, 0);
    sprites.drawSelfMasked(3, 16, left, 0);
    sprites.drawSelfMasked(93, 16, right, 0);
    sprites.drawSelfMasked(43, 56, name, 0);
    yAngle++;

    if(arduboy.justPressed(LEFT_BUTTON))
    {
        gSelection--;
    }

    if(arduboy.justPressed(RIGHT_BUTTON))
    {
        gSelection++;
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
}

void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();
    arduboy.clear();

    selection();

    arduboy.display();
}
