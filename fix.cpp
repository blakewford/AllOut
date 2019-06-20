#include <algorithm>
int32_t SDL_Init();
extern bool gKeepGoing;
void* RenderThread(void* buffer);
uint32_t gTexture[WIDTH*HEIGHT];
extern Arduboy2Base arduboy;

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEBUG_PORT 8080

uint16_t gReportedVerts = 0;

void init()
{
    if(SDL_Init() < 0) return;
}

void post()
{
        RenderThread(gTexture);
}

inline void setup();
inline void loop();

void Arduboy2Audio::begin()
{
}

void fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color)
{
// Fill a triangle - Bresenham method
// Original from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    uint8_t e1,e2;
    int8_t signx1,signx2,dx1,dy1,dx2,dy2;
    uint8_t t1x,t2x,y,minx,maxx,t1xp,t2xp;

    bool changed1 = false;
    bool changed2 = false;

    if(y1>y2) { std::swap(y1,y2); std::swap(x1,x2); }
    if(y1>y3) { std::swap(y1,y3); std::swap(x1,x3); }
    if(y2>y3) { std::swap(y2,y3); std::swap(x2,x3); }

    t1x=t2x=x1; y=y1;

    dx1 = (int8_t)(x2 - x1); if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
    dy1 = (int8_t)(y2 - y1);

    dx2 = (int8_t)(x3 - x1); if(dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
    dy2 = (int8_t)(y3 - y1);

    if(dy1 > dx1)
    {
        std::swap(dx1,dy1);
        changed1 = true;
    }
    if(dy2 > dx2)
    {
        std::swap(dy2,dx2);
        changed2 = true;
    }

    e2 = (uint8_t)(dx2>>1);

    if(y1 == y2) goto next;
    e1 = (uint8_t)(dx1>>1);

    for(uint8_t i = 0; i < dx1;)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x)
        {
            minx=t1x; maxx=t2x;
        }
        else
        {
            minx=t2x;
            maxx=t1x;
        }

        while(i<dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if(changed1) t1xp=signx1;
                else goto next1;
            }
            if(changed1) break;
            else t1x += signx1;
        }
	next1:
        while(true)
        {
            e2 += dy2;
            while(e2 >= dx2)
            {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else goto next2;
            }
            if(changed2) break;
            else t2x += signx2;
        }
	next2:
        if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
        if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
        arduboy.drawFastHLine(minx, y, maxx-minx, color);
        if(!changed1) t1x += signx1;
        t1x+=t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;

        y += 1;
        if(y == y2) break;
    }
    next:
    dx1 = (int8_t)(x3 - x2); if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
    dy1 = (int8_t)(y3 - y2);
    t1x=x2;

    if(dy1 > dx1)
    {
        std::swap(dy1,dx1);
        changed1 = true;
    } else changed1=false;

    e1 = (uint8_t)(dx1>>1);

    for(uint8_t i = 0; i<=dx1; i++)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x)
        {
            minx=t1x;
            maxx=t2x;
        }
        else
        {
            minx=t2x;
            maxx=t1x;
        }
        while(i<dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if(changed1)
                {
                    t1xp=signx1; break;
                }
                else goto next3;
            }
            if(changed1) break;
            else t1x += signx1;
            if(i < dx1) i++;
        }
	next3:
        while(t2x != x3)
        {
            e2 += dy2;
            while(e2 >= dx2)
            {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else goto next4;
            }
            if(changed2) break;
            else t2x += signx2;
        }
	next4:
        if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
        if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
        arduboy.drawFastHLine(minx, y, maxx-minx, color);

        if(!changed1) t1x += signx1;
        t1x += t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;
        y += 1;

        if(y > y3) return;
    }
}
