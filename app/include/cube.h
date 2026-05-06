#ifndef CUBE_H
#define CUBE_H

#include <stdbool.h>
#include "math3d.h"

typedef struct {
    float transform[16];
    float colors[6][3];
} SubCube;

typedef struct {
    SubCube pieces[27];
    bool is_dragging;
    int drag_axis;
    float drag_layer_coord;
    float drag_angle;
    int picked_piece_id;
} RubikCube;

void cube_init(RubikCube* cube);
void cube_snap_and_bake(RubikCube* cube);
void cube_scramble(RubikCube* cube);

#endif