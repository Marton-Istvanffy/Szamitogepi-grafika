#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "texture.h"
#include <obj/model.h>

typedef struct Scene
{
    Model cube;
    Model house;
    Model duck; 
    Material material;
    
    Material duck_material; 
    
    GLuint cube_texture_id;
    GLuint house_texture_id;
    GLuint duck_texture_id; 
    
    float house_rotation;
    vec3 house_position;
    
    float duck_rotation; 
} Scene;

void init_scene(Scene* scene);
void set_lighting(void);
void set_material(const Material* material);
void update_scene(Scene* scene, double time);
void render_scene(const Scene* scene);
void draw_origin(void);

#endif