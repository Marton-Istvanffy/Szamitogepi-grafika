#include "render.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <math.h>

static void draw_sub_cube(const SubCube* sub) {
    glBegin(GL_QUADS);
    float s = 0.48f;
    glColor3fv(sub->colors[0]); glNormal3f(0,0,1); glVertex3f(-s,-s,s); glVertex3f(s,-s,s); glVertex3f(s,s,s); glVertex3f(-s,s,s);
    glColor3fv(sub->colors[1]); glNormal3f(0,0,-1); glVertex3f(-s,-s,-s); glVertex3f(-s,s,-s); glVertex3f(s,s,-s); glVertex3f(s,-s,-s);
    glColor3fv(sub->colors[2]); glNormal3f(0,1,0); glVertex3f(-s,s,-s); glVertex3f(-s,s,s); glVertex3f(s,s,s); glVertex3f(s,s,-s);
    glColor3fv(sub->colors[3]); glNormal3f(0,-1,0); glVertex3f(-s,-s,-s); glVertex3f(s,-s,-s); glVertex3f(s,-s,s); glVertex3f(-s,-s,s);
    glColor3fv(sub->colors[4]); glNormal3f(1,0,0); glVertex3f(s,-s,-s); glVertex3f(s,s,-s); glVertex3f(s,s,s); glVertex3f(s,-s,s);
    glColor3fv(sub->colors[5]); glNormal3f(-1,0,0); glVertex3f(-s,-s,-s); glVertex3f(-s,-s,s); glVertex3f(-s,s,s); glVertex3f(-s,s,-s);
    glEnd();
}

void render_scene(const App* app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat light_pos[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    vec3 pos = app->camera.position;
    vec3 front = app->camera.front;
    vec3 up = app->camera.up;
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + front.x, pos.y + front.y, pos.z + front.z,
              up.x, up.y, up.z);

    float in = app->lighting.intensity;
    GLfloat light_amb[]  = { in * 0.7f, in * 0.7f, in * 0.7f, 1.0f }; // Alapfény minden oldalra
    GLfloat light_diff[] = { in * 0.4f, in * 0.4f, in * 0.4f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    
    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0); 
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 
    
    GLfloat fog_color[] = {0.05f, 0.05f, 0.05f, 1.0f};
    glEnable(GL_FOG); glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fog_color); glFogf(GL_FOG_DENSITY, 0.05f);

    for (int i = 0; i < 27; i++) {
        glPushMatrix();
        if (app->cube.is_dragging && app->cube.drag_axis != -1) {
            float c_pos = app->cube.pieces[i].transform[12 + app->cube.drag_axis];
            if (fabs(c_pos - app->cube.drag_layer_coord) < 0.1f) {
                if (app->cube.drag_axis == 0) glRotatef(app->cube.drag_angle, 1, 0, 0);
                if (app->cube.drag_axis == 1) glRotatef(app->cube.drag_angle, 0, 1, 0);
                if (app->cube.drag_axis == 2) glRotatef(app->cube.drag_angle, 0, 0, 1);
            }
        }
        glMultMatrixf(app->cube.pieces[i].transform);
        draw_sub_cube(&app->cube.pieces[i]);
        glPopMatrix();
    }
    
    if (!app->is_cube_mode) {
        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
        glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
        glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(-0.02f, 0.0f); glVertex2f(0.02f, 0.0f);
        glVertex2f(0.0f, -0.03f); glVertex2f(0.0f, 0.03f);
        glEnd();
        glEnable(GL_LIGHTING); glEnable(GL_DEPTH_TEST);
        glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    }
}

int get_clicked_piece(const App* app, double mx, double my, int win_w, int win_h, int fb_w, int fb_h) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING); glDisable(GL_FOG); glDisable(GL_DITHER);

    glLoadIdentity();
    vec3 pos = app->camera.position;
    vec3 front = app->camera.front;
    vec3 up = app->camera.up;
    gluLookAt(pos.x, pos.y, pos.z,
              pos.x + front.x, pos.y + front.y, pos.z + front.z,
              up.x, up.y, up.z);

    for (int i = 0; i < 27; i++) {
        glPushMatrix();
        glMultMatrixf(app->cube.pieces[i].transform);
        glColor3ub(i + 1, 0, 0); 
        float s = 0.48f;
        glBegin(GL_QUADS);
        glVertex3f(-s,-s,-s); glVertex3f(s,-s,-s); glVertex3f(s,s,-s); glVertex3f(-s,s,-s);
        glVertex3f(-s,-s,s); glVertex3f(s,-s,s); glVertex3f(s,s,s); glVertex3f(-s,s,s);
        glVertex3f(-s,-s,-s); glVertex3f(-s,-s,s); glVertex3f(-s,s,s); glVertex3f(-s,s,-s);
        glVertex3f(s,-s,-s); glVertex3f(s,-s,s); glVertex3f(s,s,s); glVertex3f(s,s,-s);
        glVertex3f(-s,s,-s); glVertex3f(s,s,-s); glVertex3f(s,s,s); glVertex3f(-s,s,s);
        glVertex3f(-s,-s,-s); glVertex3f(s,-s,-s); glVertex3f(s,-s,s); glVertex3f(-s,-s,s);
        glEnd();
        glPopMatrix();
    }

    int px, py;
    if (mx < 0.0 || my < 0.0) {
        px = fb_w / 2;
        py = fb_h / 2;
    } else {
        px = (int)(mx * ((double)fb_w / win_w));
        py = (int)((win_h - my) * ((double)fb_h / win_h));
    }
    
    unsigned char pixel[3];
    glReadPixels(px, py, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    glEnable(GL_LIGHTING); glEnable(GL_FOG); glEnable(GL_DITHER);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    if (pixel[0] > 0 && pixel[0] <= 27) return pixel[0] - 1;
    return -1;
}