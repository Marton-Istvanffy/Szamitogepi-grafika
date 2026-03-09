#ifndef SCENE_H
#define SCENE_H

#include "camera.h"

typedef struct Scene
{
    double sphere_rotation;
} Scene;

void init_scene(Scene* scene);
void update_scene(Scene* scene, double time);
void render_scene(const Scene* scene);
void draw_origin();

#endif