#include "scene.h"
#include <obj/load.h>
#include <obj/draw.h>

void init_scene(Scene* scene)
{
    load_model(&(scene->cube), "assets/models/cube.obj");
    scene->cube_texture_id = load_texture("assets/textures/cube.png");

    load_model(&(scene->house), "assets/models/house.obj");
    scene->house_texture_id = load_texture("assets/textures/house.png");

    load_model(&(scene->duck), "assets/models/kacsa.obj");
    scene->duck_texture_id = load_texture("assets/textures/duck.png");

    scene->material.ambient.red = 1.0f;
    scene->material.ambient.green = 1.0f;
    scene->material.ambient.blue = 1.0f;
    scene->material.diffuse.red = 0.5f;
    scene->material.diffuse.green = 0.8f;
    scene->material.diffuse.blue = 0.5f;
    scene->material.specular.red = 0.0f;
    scene->material.specular.green = 0.0f;
    scene->material.specular.blue = 0.0f;
    scene->material.shininess = 0.0f;

    scene->duck_rotation = 0.0f; 
    
    scene->duck_material.ambient.red = 1.0f;
    scene->duck_material.ambient.green = 1.0f;
    scene->duck_material.ambient.blue = 1.0f;
    
    scene->duck_material.diffuse.red = 1.0f;
    scene->duck_material.diffuse.green = 1.0f;
    scene->duck_material.diffuse.blue = 1.0f;
    
    scene->duck_material.specular.red = 0.0f;
    scene->duck_material.specular.green = 0.0f;
    scene->duck_material.specular.blue = 0.0f;
    scene->duck_material.shininess = 0.0f;

    scene->house_rotation = 0.0f;
    scene->house_position.x = 4.0f;
    scene->house_position.y = 0.0f;
    scene->house_position.z = 0.0f;
}

void set_lighting(void)
{
    float ambient_light[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float specular_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue,
        1.0f
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue,
        1.0f
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue,
        1.0f
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void update_scene(Scene* scene, double time)
{
    scene->house_rotation += 45.0f * time;
    if (scene->house_rotation > 360.0f) {
        scene->house_rotation -= 360.0f;
    }
    
    /* --- KACSA FORGATÁSÁNAK FRISSÍTÉSE --- */
    scene->duck_rotation += 60.0f * time; 
    
    if (scene->duck_rotation > 360.0f) {
        scene->duck_rotation -= 360.0f;
    }
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting();
    draw_origin();

    set_material(&(scene->duck_material));
    
    glBindTexture(GL_TEXTURE_2D, scene->duck_texture_id);
    glPushMatrix();
    
    glTranslatef(0.0f, 0.0f, -5.0f); 
    
    // Kacsa forgatása
    glRotatef(scene->duck_rotation, 0.0f, 0.0f, 1.0f);
    
    // Kacsa méretének állítása
    glScalef(0.5f, 0.5f, 0.5f); 
    
    draw_model(&(scene->duck));
    glPopMatrix();
}

void draw_origin(void)
{
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}