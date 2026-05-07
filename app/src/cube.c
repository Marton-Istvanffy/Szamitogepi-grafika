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
    memset(cube, 0, sizeof(RubikCube));
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
    if (cube->target_angle != 0.0f && cube->drag_axis != -1) {
        glMatrixMode(GL_MODELVIEW);
        for(int i = 0; i < 27; i++) {
            float pos = cube->pieces[i].transform[12 + cube->drag_axis];
            if (fabs(pos - cube->drag_layer_coord) < 0.1f) {
                glPushMatrix(); 
                glLoadIdentity();
                if (cube->drag_axis == 0) glRotatef(cube->target_angle, 1, 0, 0);
                if (cube->drag_axis == 1) glRotatef(cube->target_angle, 0, 1, 0);
                if (cube->drag_axis == 2) glRotatef(cube->target_angle, 0, 0, 1);
                glMultMatrixf(cube->pieces[i].transform);
                glGetFloatv(GL_MODELVIEW_MATRIX, cube->pieces[i].transform);
                glPopMatrix();
            }
        }
    }
    cube->is_dragging = false; 
    cube->drag_axis = -1; 
    cube->drag_angle = 0.0f;
    cube->current_angle = 0.0f;
    cube->target_angle = 0.0f;
}

void cube_start_anim(RubikCube* cube) {
    if (!cube->is_dragging || cube->drag_axis == -1) return;
    cube->target_angle = round(cube->drag_angle / 90.0f) * 90.0f;
    cube->current_angle = cube->drag_angle;
    cube->is_animating = true;
    cube->is_dragging = false; 
}

bool cube_update_anim(RubikCube* cube, float dt) {
    if (!cube->is_animating) return false;
    
    float diff = cube->target_angle - cube->current_angle;
    float step = 400.0f * dt; 
    
    if (fabs(diff) <= step) {
        cube->current_angle = cube->target_angle;
        cube->drag_angle = cube->current_angle;
        cube_snap_and_bake(cube);
        cube->is_animating = false;
        return true; 
    } else {
        cube->current_angle += (diff > 0 ? step : -step);
        cube->drag_angle = cube->current_angle;
        return false;
    }
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

bool cube_is_solved(const RubikCube* cube) {
    float dirs[6][3] = { {0,0,1}, {0,0,-1}, {0,1,0}, {0,-1,0}, {1,0,0}, {-1,0,0} };

    for (int d = 0; d < 6; d++) {
        float* dir = dirs[d];
        float face_color[3] = {-1, -1, -1}; 

        for (int i = 0; i < 27; i++) {
            const SubCube* p = &cube->pieces[i];
            float pos[3] = {p->transform[12], p->transform[13], p->transform[14]};

            if (pos[0]*dir[0] + pos[1]*dir[1] + pos[2]*dir[2] > 0.5f) {
                
                int local_f = -1;
                for (int f = 0; f < 6; f++) {
                    float* ldir = dirs[f];
                    float w_nx = p->transform[0]*ldir[0] + p->transform[4]*ldir[1] + p->transform[8]*ldir[2];
                    float w_ny = p->transform[1]*ldir[0] + p->transform[5]*ldir[1] + p->transform[9]*ldir[2];
                    float w_nz = p->transform[2]*ldir[0] + p->transform[6]*ldir[1] + p->transform[10]*ldir[2];

                    if (w_nx*dir[0] + w_ny*dir[1] + w_nz*dir[2] > 0.5f) {
                        local_f = f; break;
                    }
                }

                if (local_f != -1) {
                    float r = p->colors[local_f][0], g = p->colors[local_f][1], b = p->colors[local_f][2];
                    if (r < 0.2f && g < 0.2f && b < 0.2f) continue; 
                    
                    if (face_color[0] < 0) { 
                        face_color[0] = r; face_color[1] = g; face_color[2] = b;
                    } else { 
                        if (fabs(face_color[0]-r) > 0.1f || fabs(face_color[1]-g) > 0.1f || fabs(face_color[2]-b) > 0.1f) {
                            return false; 
                        }
                    }
                }
            }
        }
    }
    return true; 
}