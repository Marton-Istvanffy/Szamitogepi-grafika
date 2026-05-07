#ifndef APP_H
#define APP_H

#include "camera.h"
#include "cube.h"
#include "particles.h"

typedef struct { float intensity; } Lighting;

typedef struct {
    Camera camera;
    RubikCube cube;
    Lighting lighting;
    ParticleSystem particle_sys;
    
    bool keys[1024];
    double last_time;
    double delta_time;
    
    float last_mouse_x;
    float last_mouse_y;
    bool first_mouse;

    float player_y;
    float velocity_y;
    bool is_grounded;
    bool is_crouching;

    bool is_cube_mode; 
    bool right_mouse_pressed;
    float orbit_distance;
    
    int step_count;
    bool is_scrambled;
} App;

#endif