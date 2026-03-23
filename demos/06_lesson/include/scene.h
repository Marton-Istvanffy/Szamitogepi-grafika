#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"
#include <obj/model.h>

typedef struct Scene
{
    Model cube;
    Material material;
    GLuint texture_id;
} Scene;

void init_scene(Scene* scene);
void set_lighting(void);
void set_material(const Material* material);
void update_scene(Scene* scene);
void render_scene(const Scene* scene);
void draw_origin(void);

#endif