#ifndef MATH3D_H
#define MATH3D_H

typedef struct {
    float x, y, z;
} vec3;

void math3d_lookAt(vec3 eye, vec3 center, vec3 up);

#endif