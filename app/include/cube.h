#ifndef CUBE_H
#define CUBE_H

#include <stdbool.h>

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
    float target_angle;
    float current_angle;
    bool is_animating;
    int picked_piece_id;
} RubikCube;

void cube_init(RubikCube* cube);
void cube_start_anim(RubikCube* cube);
bool cube_update_anim(RubikCube* cube, float dt);
void cube_scramble(RubikCube* cube);
bool cube_is_solved(const RubikCube* cube);

#endif