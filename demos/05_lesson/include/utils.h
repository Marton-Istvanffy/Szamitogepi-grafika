#ifndef UTILS_H
#define UTILS_H

typedef struct vec3
{
    float x;
    float y;
    float z;
} vec3;

typedef struct Color
{
    float red;
    float green;
    float blue;
} Color;

double degree_to_radian(double degree);

#endif