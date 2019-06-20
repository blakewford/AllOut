#ifndef Models_h
#define Models_h

#include <string.h>
#include <Arduboy2.h>

const uint8_t BUFFER_SIZE = 16;

struct param
{
    float value[BUFFER_SIZE];
    int8_t shape[2];

    param()
    {
        memset(value, '\0', sizeof(float)*BUFFER_SIZE);

        // Most commonly used size
        shape[0] = 3;
        shape[1] = 1;
    }
};

enum rotation_axis: int8_t
{
    X,
    Y,
    Z
};

class Models
{
public:
    static void begin();
    static void drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);
    static void drawCompressedModel(const uint8_t* model, const float* map, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);

private:

    static param s_Ortho;
    static param s_zAngle;

    static void drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);
    static void modifyAngle(const int16_t angle, const rotation_axis axis);
    static void modifyXAngle();
};

#endif
