#ifndef CAMERA_H
#define CAMERA_H

#include "math3d.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
} Camera;

void camera_init(Camera* cam);
void camera_update_fps(Camera* cam, float deltaTime, const Uint8* keys, float* player_y, float* velocity_y, bool* is_grounded, bool is_crouching);
void camera_process_mouse(Camera* cam, float xoffset, float yoffset);

#endif