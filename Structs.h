#pragma once

#include <cinttypes>
#include <cstdint>
#include <math.h>

namespace rc {

typedef struct Image
{
    uint8_t* data;
    uint32_t h, w;
    uint8_t comp;
} Image;

typedef struct RGBA
{
    uint8_t r, g, b, a;
} RGBA;

typedef struct Square
{
    double mean;
    double deviation;
    float x, y, w, h;
} Square;

typedef struct Point
{
    float x, y;
    double deviation;
} Point;

}
