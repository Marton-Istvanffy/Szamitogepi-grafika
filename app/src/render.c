#include "app.h"
#include "math3d.h"
#include "obj/draw.h"
#include <SDL2/SDL_opengl.h>
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

static void draw_environment(const App* app) {
    if (app->scene.is_cube_mode) return;
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); 

    float floor_y = -2.01f;
    float s = 15.0f;           
    float h = 15.0f;          
    float ceil_y = floor_y + h;
    float rep = 5.0f;         

    if (app->scene.floor_texture_id != 0) {
        glBindTexture(GL_TEXTURE_2D, app->scene.floor_texture_id);
    }
    
    glNormal3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, floor_y, -s);
        glTexCoord2f(rep,  0.0f); glVertex3f( s, floor_y, -s);
        glTexCoord2f(rep,  rep);  glVertex3f( s, floor_y,  s);
        glTexCoord2f(0.0f, rep);  glVertex3f(-s, floor_y,  s);
    glEnd();

    glNormal3f(0.0f, -1.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, ceil_y,  s);
        glTexCoord2f(rep,  0.0f); glVertex3f( s, ceil_y,  s);
        glTexCoord2f(rep,  rep);  glVertex3f( s, ceil_y, -s);
        glTexCoord2f(0.0f, rep);  glVertex3f(-s, ceil_y, -s);
    glEnd();

    if (app->scene.wall_texture_id != 0) {
        glBindTexture(GL_TEXTURE_2D, app->scene.wall_texture_id);
    }

    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, floor_y, -s);
        glTexCoord2f(rep,  0.0f); glVertex3f( s, floor_y, -s);
        glTexCoord2f(rep,  rep);  glVertex3f( s, ceil_y,  -s);
        glTexCoord2f(0.0f, rep);  glVertex3f(-s, ceil_y,  -s);
    glEnd();

    glNormal3f(0.0f, 0.0f, -1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( s, floor_y,  s);
        glTexCoord2f(rep,  0.0f); glVertex3f(-s, floor_y,  s);
        glTexCoord2f(rep,  rep);  glVertex3f(-s, ceil_y,   s);
        glTexCoord2f(0.0f, rep);  glVertex3f( s, ceil_y,   s);
    glEnd();

    glNormal3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, floor_y,  s);
        glTexCoord2f(rep,  0.0f); glVertex3f(-s, floor_y, -s);
        glTexCoord2f(rep,  rep);  glVertex3f(-s, ceil_y,  -s);
        glTexCoord2f(0.0f, rep);  glVertex3f(-s, ceil_y,   s);
    glEnd();

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( s, floor_y, -s);
        glTexCoord2f(rep,  0.0f); glVertex3f( s, floor_y,  s);
        glTexCoord2f(rep,  rep);  glVertex3f( s, ceil_y,   s);
        glTexCoord2f(0.0f, rep);  glVertex3f( s, ceil_y,  -s);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopAttrib();
}

void render_scene(const App* app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    vec3 pos = app->player.camera.position;
    vec3 front = app->player.camera.front;
    vec3 up = app->player.camera.up;

    vec3 center = {pos.x + front.x, pos.y + front.y, pos.z + front.z};
    math3d_lookAt(pos, center, up);

    float in = app->scene.light_intensity;
    GLfloat light_pos[] = { 0.0f, 4.0f, 0.0f, 1.0f }; 
    GLfloat light_amb[]  = { in * 0.25f, in * 0.25f, in * 0.25f, 1.0f };
    GLfloat light_diff[] = { in, in, in, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f); 
    
    glEnable(GL_LIGHTING); 
    glEnable(GL_LIGHT0); 
    glEnable(GL_COLOR_MATERIAL);
    
    GLfloat fog_color[] = {0.02f, 0.02f, 0.02f, 1.0f};
    glEnable(GL_FOG); 
    glFogfv(GL_FOG_COLOR, fog_color); 
    glFogf(GL_FOG_DENSITY, 0.06f); 
    glFogi(GL_FOG_MODE, GL_EXP2);

    draw_environment(app);

    if (app->scene.lamp_model.n_vertices > 0) {
        glPushMatrix();
        glTranslatef(3.0f, -2.01f, 0.0f);
        
        glEnable(GL_TEXTURE_2D);
        if (app->scene.metal_texture_id != 0) {
            glBindTexture(GL_TEXTURE_2D, app->scene.metal_texture_id);
        }
        glColor3f(1.0f, 1.0f, 1.0f);
        
        draw_model(&app->scene.lamp_model);
        
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    for (int i = 0; i < 27; i++) {
        glPushMatrix();
        if ((app->scene.cube.is_dragging || app->scene.cube.is_animating) && app->scene.cube.drag_axis != -1) {
            float c_pos = app->scene.cube.pieces[i].transform[12 + app->scene.cube.drag_axis];
            if (fabs(c_pos - app->scene.cube.drag_layer_coord) < 0.1f) {
                if (app->scene.cube.drag_axis == 0) glRotatef(app->scene.cube.drag_angle, 1, 0, 0);
                if (app->scene.cube.drag_axis == 1) glRotatef(app->scene.cube.drag_angle, 0, 1, 0);
                if (app->scene.cube.drag_axis == 2) glRotatef(app->scene.cube.drag_angle, 0, 0, 1);
            }
        }
        glMultMatrixf(app->scene.cube.pieces[i].transform);
        draw_sub_cube(&app->scene.cube.pieces[i]);
        glPopMatrix();
    }
    
    particles_render(&app->scene.particle_sys);
    
    if (!app->scene.is_cube_mode) {
        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
        glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
        glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_FOG);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES); 
        glVertex2f(-0.02f, 0.0f); glVertex2f(0.02f, 0.0f); 
        glVertex2f(0.0f, -0.03f); glVertex2f(0.0f, 0.03f); 
        glEnd();
        glEnable(GL_FOG); glEnable(GL_LIGHTING); glEnable(GL_DEPTH_TEST);
        glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    }
}

int get_clicked_piece(const App* app, double mx, double my, int win_w, int win_h, int fb_w, int fb_h) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING); glDisable(GL_FOG); glDisable(GL_DITHER);
    glLoadIdentity();
    
    vec3 pos = app->player.camera.position; 
    vec3 front = app->player.camera.front; 
    vec3 up = app->player.camera.up; 
    
    vec3 center = {pos.x + front.x, pos.y + front.y, pos.z + front.z};
    math3d_lookAt(pos, center, up);

    for (int i = 0; i < 27; i++) {
        glPushMatrix(); glMultMatrixf(app->scene.cube.pieces[i].transform);
        glColor3ub(i + 1, 0, 0); 
        float s = 0.48f;
        glBegin(GL_QUADS);
        glVertex3f(-s,-s,-s); glVertex3f(s,-s,-s); glVertex3f(s,s,-s); glVertex3f(-s,s,-s);
        glVertex3f(-s,-s,s); glVertex3f(s,-s,s); glVertex3f(s,s,s); glVertex3f(-s,s,s);
        glVertex3f(-s,-s,-s); glVertex3f(-s,-s,s); glVertex3f(-s,s,s); glVertex3f(-s,s,-s);
        glVertex3f(s,-s,-s); glVertex3f(s,-s,s); glVertex3f(s,s,s); glVertex3f(s,s,-s);
        glVertex3f(-s,s,-s); glVertex3f(s,s,-s); glVertex3f(s,s,s); glVertex3f(-s,s,s);
        glVertex3f(-s,-s,-s); glVertex3f(s,-s,-s); glVertex3f(s,-s,s); glVertex3f(-s,-s,s);
        glEnd(); glPopMatrix();
    }

    int px = (mx < 0.0) ? fb_w/2 : (int)(mx * ((double)fb_w/win_w));
    int py = (mx < 0.0) ? fb_h/2 : (int)((win_h - my) * ((double)fb_h/win_h));
    
    unsigned char pixel[3];
    glReadPixels(px, py, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    
    glEnable(GL_LIGHTING); glEnable(GL_FOG); glEnable(GL_DITHER);
    
    if (pixel[0] > 0 && pixel[0] <= 27) return pixel[0] - 1;
    return -1;
}