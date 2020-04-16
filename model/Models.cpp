#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include <map>
#include <vector>
#include <thread>
#include <chrono>
using namespace std::chrono;

#include "Models.h"
#include "../tools/bmpconstants.h"

extern Arduboy2Base arduboy;
extern uint16_t gReportedVerts;

#define USE_TEXTURE

enum parse_state: int8_t
{
    CONSTANTS,
    SHAPE,
    NAME,
    UNKNOWN
};

void MatMul3x1(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]) + (A[1]*B[1]) + (A[2]*B[2]);
    C[1] = (A[3]*B[0]) + (A[4]*B[1]) + (A[5]*B[2]);
    C[2] = (A[6]*B[0]) + (A[7]*B[1]) + (A[8]*B[2]);
}

void MatMul4x1(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]) + (A[1]*B[1]) + (A[2]*B[2]) + (A[3]*B[3]);
    C[1] = (A[4]*B[0]) + (A[5]*B[1]) + (A[6]*B[2]) + (A[7]*B[3]);
    C[2] = (A[8]*B[0]) + (A[9]*B[1]) + (A[10]*B[2]) + (A[11]*B[3]);
    C[3] = (A[12]*B[0]) + (A[13]*B[1]) + (A[14]*B[2]) + (A[15]*B[3]);
}

void TensorPort(const param& A, const param& B, float* C)
{
    assert(A.shape[1] == B.shape[0]);
    if(A.shape[0] == 3)
    {
        MatMul3x1(C, (float*)A.value, (float*)B.value);
    }
    else
    {
        MatMul4x1(C, (float*)A.value, (float*)B.value);
    }
}

param X_AT_15_DEGREES;

void rotationEntry(const int16_t angle, param& parameter, rotation_axis axis)
{
    const float radians = (angle%360)*0.0174533;

//    parameter.shape[0] = 3;
    parameter.shape[1] = 3;

    const float sine = sin(radians);
    const float cosine = cos(radians);

    switch(axis)
    {
        case X:
            parameter.value[0] = 1;
/*
            parameter.value[1] = 0;
            parameter.value[2] = 0;
            parameter.value[3] = 0;
*/
            parameter.value[4] = cosine;
            parameter.value[5] = sine;
//            parameter.value[6] = 0;
            parameter.value[7] = -sine;
            parameter.value[8] = cosine;
            break;
        case Y:
            parameter.value[0] = cosine;
//            parameter.value[1] = 0;
            parameter.value[2] = -sine;
//            parameter.value[3] = 0;
            parameter.value[4] = 1;
//            parameter.value[5] = 0;
            parameter.value[6] = sine;
//            parameter.value[7] = 0;
            parameter.value[8] = cosine;
            break;
/*
        case Z:
            parameter.value[0] = cos(radians);
            parameter.value[1] = sin(radians);
            parameter.value[2] = 0;
            parameter.value[3] = -sin(radians);
            parameter.value[4] = cos(radians);
            parameter.value[5] = 0;
            parameter.value[6] = 0;
            parameter.value[7] = 0;
            parameter.value[8] = 1;
            break;
*/
        default:
            break;
    }
}

float ortho[4][4] =
{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

struct vertex
{
    float x;
    float y;
    float z;
};

float copy[1024];

param Models::s_Ortho;
param Models::s_zAngle;

void Models::begin()
{
//    rotationEntry(zAngle, s_zAngle, Z);
    s_zAngle.value[0] = 1;
//    s_zAngle.value[1] = 0;
//    s_zAngle.value[2] = 0;
//    s_zAngle.value[3] = -0;
    s_zAngle.value[4] = 1;
//    s_zAngle.value[5] = 0;
//    s_zAngle.value[6] = 0;
//    s_zAngle.value[7] = 0;
    s_zAngle.value[8] = 1;
//    s_zAngle.shape[0] = 3;
    s_zAngle.shape[1] = 3;

    s_Ortho.shape[0] = 4;
    s_Ortho.shape[1] = 4;
    memcpy(s_Ortho.value, ortho, 16*sizeof(float));

    X_AT_15_DEGREES.value[0] = 1;
//    X_AT_15_DEGREES.value[1] = 0;
//    X_AT_15_DEGREES.value[2] = 0;
//    X_AT_15_DEGREES.value[3] = 0;
    X_AT_15_DEGREES.value[4] = 0.965925813;
    X_AT_15_DEGREES.value[5] = 0.258819163;
    X_AT_15_DEGREES.value[6] = -0.258819163;
    X_AT_15_DEGREES.value[7] = 0.965925813;
//    X_AT_15_DEGREES.value[8] = 0;
//    X_AT_15_DEGREES.shape[0] = 3;
    X_AT_15_DEGREES.shape[1] = 3;
}

void Models::drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
}

void debugWait()
{
#if 0
            post();
            system_clock::time_point wait = system_clock::now() + milliseconds(2);
            while(system_clock::now() < wait)
            {
                std::this_thread::yield();
            }
#endif
}

void Models::drawCompressedModel(const uint8_t* model, const float* map, const uint8_t* fill, const int8_t* order, int16_t xAngle, int16_t yAngle, int16_t zAngle)
{
    int16_t count = (int16_t)map[0];
    copy[0] = count;
    count*=3;

    gReportedVerts += count;

    bool reverse = (yAngle%360 < 90) || (yAngle%360 > 270);

    int16_t current = 1;
    int16_t ndx = reverse ? 0: count-1;
    if(reverse)
    {
        while(ndx < count)
        {
            copy[current++] = map[pgm_read_byte(&model[ndx++])];
        }
    }
    else
    {
        yAngle += 270;
        while(ndx >= 0)
        {
            copy[current++] = map[pgm_read_byte(&model[ndx--])];
        }
    }

    drawModel(xAngle, yAngle, zAngle, fill, order, reverse);
    debugWait();
}

void Models::modifyAngle(const int16_t angle, const rotation_axis axis)
{
    param A, B;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];

    rotationEntry(angle, A, axis);
    while(count--)
    {
        int16_t start = current;
        B.value[2] = copy[current++];
        B.value[1] = copy[current++];
        B.value[0] = copy[current++];
//        B.shape[0] = 3;
//        B.shape[1] = 1;
        float C[(int8_t)(A.shape[0]*B.shape[1])];
        TensorPort(A, B, C);
        memcpy(&copy[start], &C[0], 3*sizeof(float));
    }
}

void Models::drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t* color, int8_t* order, bool reverse)
{
    uint8_t* binary = nullptr;
    uint8_t* nextColor = nullptr;

#ifdef USE_TEXTURE
    FILE* skin = fopen("./tools/skin.bmp","rb");
    if(skin)
    {
        fseek(skin, 0, SEEK_END);
        int32_t size = ftell(skin);
        rewind(skin);
        binary = (uint8_t*)malloc(size);
        size_t read = fread(binary, 1, size, skin);
        if(read != size) return -1;
        fclose(skin);

        nextColor = binary + HEADERSIZE; // Bitmap header size
    }
#endif

    modifyAngle(yAngle, Y);
    modifyAngle(xAngle, X);
//    modifyAngle(zAngle, Z);

    param H;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];
    while(count--)
    {
        int16_t start = current;
        H.value[2] = copy[current++];
        H.value[1] = copy[current++];
        H.value[0] = copy[current++];
//        H.shape[0] = 3;
//        H.shape[1] = 1;
        float I[(int8_t)(s_zAngle.shape[0]*H.shape[1])];
        TensorPort(s_zAngle, H, I);
        memcpy(&copy[start], &I[0], 3*sizeof(float));
    }

    current = 1;
    count = (int16_t)copy[0];
    while(count--)
    {
        param K;
        int16_t start = current;
        K.value[3] = 1.0f;
        K.value[2] = copy[current++];
        K.value[1] = copy[current++];
        K.value[0] = copy[current++];
        K.shape[0] = 4;
//        K.shape[1] = 1;

        float L[(int8_t)(s_Ortho.shape[0]*K.shape[1])];
        TensorPort(s_Ortho, K, L);
        memcpy(&copy[start], &L[0], 3*sizeof(float));
    }

    int16_t offsetX = WIDTH/2;
    int16_t offsetY = HEIGHT/2;

    current = 1;
    count = (int16_t)copy[0];

    std::vector<triangle> temp;
    std::map<int32_t, triangle> triangles;

    int32_t total = count*3;
    while(current < total)
    {
        triangle t;
        t.texture = 0;

        if(!reverse)
        {
           int32_t i = (current-1)/3;
           t.color = color[i];
           t.order = order[i];
           if(skin)
           {
//               t.texture = nextColor + texture[i/3];
           }
        }

        t.a.x = copy[current++] + offsetX;
        t.a.y = copy[current++] + offsetY;
        current++;
        t.b.x = copy[current++] + offsetX;
        t.b.y = copy[current++] + offsetY;
        current++;
        t.c.x = copy[current++] + offsetX;
        t.c.y = copy[current++] + offsetY;
        current++;

        if(reverse)
        {
           int32_t i = (total - (current-1))/3;
           t.color = color[i];
           t.order = order[i];
           if(skin)
           {
//               t.texture = nextColor + texture[i/3];
           }
        }

        temp.push_back(t);
    }

    for(std::vector<triangle>::iterator i = temp.begin(); i != temp.end(); i++)
    {
        if(i->order != -1)
        {
            triangles.insert(std::make_pair(i->order, *i));
        }
    }

    int32_t ndx = 0;
    for(std::vector<triangle>::iterator i = temp.begin(); i != temp.end(); i++)
    {
        if(triangles.find(ndx) != triangles.end())
        {
            while(triangles.find(ndx) != triangles.end())
            {
               ndx++;
            }
        }

        if(i->order == -1)
        {
            triangles.insert(std::make_pair(ndx, *i));
        }   
    }

    int32_t z = 0;
    while(z < triangles.size())
    {
        triangle* p = &triangles[z];
        fillTriangle(*p);
        z++;
    }

#ifdef USE_TEXTURE
    if(skin)
    {
        free(binary);
        binary = nullptr;
    }
#endif
}