#include "cube.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>

static void set_identity(float* m) {
    memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void set_color(float dest[3], float r, float g, float b) {
    dest[0] = r; dest[1] = g; dest[2] = b;
}

void cube_init(RubikCube* cube) {
    cube->is_dragging = false;
    cube->drag_axis = -1;
    float col_black[3]  = {0.1f, 0.1f, 0.1f};
    float col_red[3]    = {0.8f, 0.0f, 0.0f};
    float col_orange[3] = {1.0f, 0.4f, 0.0f};
    float col_white[3]  = {0.9f, 0.9f, 0.9f};
    float col_yellow[3] = {0.9f, 0.9f, 0.0f};
    float col_blue[3]   = {0.0f, 0.0f, 0.8f};
    float col_green[3]  = {0.0f, 0.6f, 0.0f};
    
    int index = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                set_identity(cube->pieces[index].transform);
                cube->pieces[index].transform[12] = x * 1.05f;
                cube->pieces[index].transform[13] = y * 1.05f;
                cube->pieces[index].transform[14] = z * 1.05f;
                if (z == 1)  set_color(cube->pieces[index].colors[0], col_red[0], col_red[1], col_red[2]);
                else         set_color(cube->pieces[index].colors[0], col_black[0], col_black[1], col_black[2]);
                if (z == -1) set_color(cube->pieces[index].colors[1], col_orange[0], col_orange[1], col_orange[2]);
                else         set_color(cube->pieces[index].colors[1], col_black[0], col_black[1], col_black[2]);
                if (y == 1)  set_color(cube->pieces[index].colors[2], col_white[0], col_white[1], col_white[2]);
                else         set_color(cube->pieces[index].colors[2], col_black[0], col_black[1], col_black[2]);
                if (y == -1) set_color(cube->pieces[index].colors[3], col_yellow[0], col_yellow[1], col_yellow[2]);
                else         set_color(cube->pieces[index].colors[3], col_black[0], col_black[1], col_black[2]);
                if (x == 1)  set_color(cube->pieces[index].colors[4], col_blue[0], col_blue[1], col_blue[2]);
                else         set_color(cube->pieces[index].colors[4], col_black[0], col_black[1], col_black[2]);
                if (x == -1) set_color(cube->pieces[index].colors[5], col_green[0], col_green[1], col_green[2]);
                else         set_color(cube->pieces[index].colors[5], col_black[0], col_black[1], col_black[2]);
                index++;
            }
        }
    }
}

void cube_snap_and_bake(RubikCube* cube) {
    if (!cube->is_dragging || cube->drag_axis == -1) return;
    float snapped_angle = round(cube->drag_angle / 90.0f) * 90.0f;
    if (snapped_angle != 0.0f) {
        glMatrixMode(GL_MODELVIEW);
        for(int i=0; i<27; i++) {
            float pos = cube->pieces[i].transform[12 + cube->drag_axis];
            if (fabs(pos - cube->drag_layer_coord) < 0.1f) {
                glPushMatrix(); glLoadIdentity();
                if (cube->drag_axis == 0) glRotatef(snapped_angle, 1, 0, 0);
                if (cube->drag_axis == 1) glRotatef(snapped_angle, 0, 1, 0);
                if (cube->drag_axis == 2) glRotatef(snapped_angle, 0, 0, 1);
                glMultMatrixf(cube->pieces[i].transform);
                glGetFloatv(GL_MODELVIEW_MATRIX, cube->pieces[i].transform);
                glPopMatrix();
            }
        }
    }
    cube->is_dragging = false; cube->drag_axis = -1; cube->drag_angle = 0.0f;
}

void cube_scramble(RubikCube* cube) {
    float layers[] = {-1.05f, 0.0f, 1.05f};
    for (int step = 0; step < 30; step++) {
        int axis = rand() % 3;
        float l_coord = layers[rand() % 3];
        float angle = ((rand() % 3) + 1) * 90.0f;
        glMatrixMode(GL_MODELVIEW);
        for(int i=0; i<27; i++) {
            float pos = cube->pieces[i].transform[12 + axis];
            if (fabs(pos - l_coord) < 0.1f) {
                glPushMatrix(); glLoadIdentity();
                if (axis == 0) glRotatef(angle, 1, 0, 0);
                if (axis == 1) glRotatef(angle, 0, 1, 0);
                if (axis == 2) glRotatef(angle, 0, 0, 1);
                glMultMatrixf(cube->pieces[i].transform);
                glGetFloatv(GL_MODELVIEW_MATRIX, cube->pieces[i].transform);
                glPopMatrix();
            }
        }
    }
}