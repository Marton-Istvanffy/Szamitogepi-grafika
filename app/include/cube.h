#ifndef CUBE_H
#define CUBE_H

#include <cglm/cglm.h>

typedef struct {
    vec3 position;          // A kocka aktuális helye a 3x3-as rácsban
    mat4 modelMatrix;       // A kocka saját transzformációs mátrixa (pozíció + forgatás)
    unsigned int vao;       // A kocka geometriája
} SubCube;

typedef struct {
    SubCube subcubes[27];
    int isAnimating;
    float animationAngle;
} RubiksCube;

void init_rubiks_cube(RubiksCube* cube);
void rotate_face(RubiksCube* cube, int faceIndex, float angle); // faceIndex: pl. 0=Top, 1=Bottom, stb.

#endif