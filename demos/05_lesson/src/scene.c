#include "scene.h"

#include <GL/gl.h>
#include <math.h>

void init_scene(Scene* scene)
{
    scene->sphere_rotation = 0.0;
}

void update_scene(Scene* scene, double time)
{
    scene->sphere_rotation += 45.0 * time;
}

void render_scene(const Scene* scene)
{
    int i, j;
    int slices = 24, stacks = 24;
    float r = 0.15;
    float angle;

    draw_origin();

    glBegin(GL_QUADS);
    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 8; ++j) {
            if ((i + j) % 2 == 0) {
                glColor3f(1.0, 1.0, 1.0);
            } else {
                glColor3f(0.0, 0.0, 0.0);
            }
            glVertex3f(i / 8.0, j / 8.0, 0.0);
            glVertex3f((i + 1) / 8.0, j / 8.0, 0.0);
            glVertex3f((i + 1) / 8.0, (j + 1) / 8.0, 0.0);
            glVertex3f(i / 8.0, (j + 1) / 8.0, 0.0);
        }
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 0.0);
    glVertex3f(0.2, 0.2, 0.1);
    glColor3f(0.0, 1.0, 1.0);
    glVertex3f(0.8, 0.2, 0.1);
    glColor3f(1.0, 0.0, 1.0);
    glVertex3f(0.5, 0.8, 0.1);
    glEnd();

    glPushMatrix();
    glTranslatef(0.5, 0.5, 0.5);
    glRotatef(scene->sphere_rotation, 0.0, 0.0, 1.0);
    glColor3f(0.3, 0.0, 0.5);
    for (i = 0; i < stacks; ++i) {
        float lat0 = M_PI * (-0.5 + (float)i / stacks);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);

        float lat1 = M_PI * (-0.5 + (float)(i + 1) / stacks);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= slices; ++j) {
            float lng = 2 * M_PI * (float)j / slices;
            float x = cos(lng);
            float y = sin(lng);

            glVertex3f(r * x * zr0, r * y * zr0, r * z0);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2, 0.8, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= 16; ++i) {
        angle = i * 2.0 * M_PI / 16.0;
        glVertex3f(0.1 * cos(angle), 0.1 * sin(angle), 0.3);
        glVertex3f(0.1 * cos(angle), 0.1 * sin(angle), 0.0);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.8, 0.8, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.0, 0.3);
    for (i = 0; i <= 16; ++i) {
        angle = i * 2.0 * M_PI / 16.0;
        glVertex3f(0.1 * cos(angle), 0.1 * sin(angle), 0.0);
    }
    glEnd();
    glPopMatrix();
}

void draw_origin()
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);
    glEnd();
}