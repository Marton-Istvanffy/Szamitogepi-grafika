#include "app.h"
#include "math3d.h"
#include "obj/draw.h"
#include <SDL2/SDL_opengl.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    float x, y, z;
    GLenum light_id;
} CandlePlacement;

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

static void draw_box(float x, float y, float z, float sx, float sy, float sz) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    glNormal3f(0,0,1); glTexCoord2f(0,0); glVertex3f(-1,-1,1); glTexCoord2f(1,0); glVertex3f(1,-1,1); glTexCoord2f(1,1); glVertex3f(1,1,1); glTexCoord2f(0,1); glVertex3f(-1,1,1);
    glNormal3f(0,0,-1); glTexCoord2f(0,0); glVertex3f(1,-1,-1); glTexCoord2f(1,0); glVertex3f(-1,-1,-1); glTexCoord2f(1,1); glVertex3f(-1,1,-1); glTexCoord2f(0,1); glVertex3f(1,1,-1);
    glNormal3f(0,1,0); glTexCoord2f(0,0); glVertex3f(-1,1,1); glTexCoord2f(1,0); glVertex3f(1,1,1); glTexCoord2f(1,1); glVertex3f(1,1,-1); glTexCoord2f(0,1); glVertex3f(-1,1,-1);
    glNormal3f(0,-1,0); glTexCoord2f(0,0); glVertex3f(-1,-1,-1); glTexCoord2f(1,0); glVertex3f(1,-1,-1); glTexCoord2f(1,1); glVertex3f(1,-1,1); glTexCoord2f(0,1); glVertex3f(-1,-1,1);
    glNormal3f(1,0,0); glTexCoord2f(0,0); glVertex3f(1,-1,1); glTexCoord2f(1,0); glVertex3f(1,-1,-1); glTexCoord2f(1,1); glVertex3f(1,1,-1); glTexCoord2f(0,1); glVertex3f(1,1,1);
    glNormal3f(-1,0,0); glTexCoord2f(0,0); glVertex3f(-1,-1,-1); glTexCoord2f(1,0); glVertex3f(-1,-1,1); glTexCoord2f(1,1); glVertex3f(-1,1,1); glTexCoord2f(0,1); glVertex3f(-1,1,-1);
    glEnd();
    glPopMatrix();
}

// Render fire effect
static void draw_fire_particles(float cx, float cy, float cz, double time, float scale) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    
    glPushMatrix();
    glTranslatef(cx, cy, cz);
    
    srand((unsigned int)(fabs(cx) * 73.0f + fabs(cz) * 37.0f)); 
    
    glBegin(GL_QUADS);
    for (int i = 0; i < 150; i++) {
        float s1 = (rand() % 1000) / 1000.0f; 
        float s2 = (rand() % 1000) / 1000.0f;
        float s3 = (rand() % 1000) / 1000.0f;
        
        float speed = 1.0f + s1 * 2.0f;
        float life = fmod((time * speed) + s2, 1.0f); 
        
        float size = 0.7f * (1.0f - life) * scale;
        float px = (s1 - 0.5f) * 2.0f * (1.0f - life) * scale; 
        float pz = (s3 - 0.5f) * 2.0f * (1.0f - life) * scale;
        float py = life * 6.0f * scale; 
        
        glColor4f(1.0f, 1.0f - life, 0.1f, 1.0f - life);
        
        glVertex3f(px - size, py, pz);
        glVertex3f(px + size, py, pz);
        glVertex3f(px + size, py + size*2, pz);
        glVertex3f(px - size, py + size*2, pz);
        
        glVertex3f(px, py, pz - size);
        glVertex3f(px, py, pz + size);
        glVertex3f(px, py + size*2, pz + size);
        glVertex3f(px, py + size*2, pz - size);
    }
    glEnd();
    
    glPopMatrix();
    glDepthMask(GL_TRUE);
    glPopAttrib();
}

static void draw_sky(const App* app) {
    if (!app->scene.sky_texture_id) return;
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, app->scene.sky_texture_id);
    glColor3f(1.0f, 1.0f, 1.0f);
    
    glPushMatrix();
    glTranslatef(app->player.camera.position.x, app->player.camera.position.y, app->player.camera.position.z);
    
    float d = 40.0f;
    
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, -d, -d);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( d, -d, -d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( d,  d, -d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-d,  d, -d);
    
    glTexCoord2f(0.0f, 1.0f); glVertex3f( d, -d, -d);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( d, -d,  d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( d,  d,  d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( d,  d, -d);
    
    glTexCoord2f(0.0f, 1.0f); glVertex3f( d, -d,  d);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-d, -d,  d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-d,  d,  d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( d,  d,  d);
    
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, -d,  d);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-d, -d, -d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-d,  d, -d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-d,  d,  d);
    
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-d,  d, -d);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( d,  d, -d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( d,  d,  d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-d,  d,  d);
    
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-d, -d,  d);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( d, -d,  d);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( d, -d, -d);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-d, -d, -d);
    glEnd();
    
    glPopMatrix();
    glPopAttrib();
}

// Render room environment
static void draw_environment(const App* app) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); 
    float floor_y = -2.01f;
    float s = 15.0f;
    float h = 15.0f;
    float ceil_y = floor_y + h;
    float rep = 5.0f;
    
    if (app->scene.floor_texture_id != 0) glBindTexture(GL_TEXTURE_2D, app->scene.floor_texture_id);
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
    
    if (app->scene.wall_texture_id != 0) glBindTexture(GL_TEXTURE_2D, app->scene.wall_texture_id);
    
    float hx1 = -1.5f, hx2 = 1.5f;
    float hy = floor_y + 5.0f;
    
    glNormal3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, floor_y, -s);
    glTexCoord2f((s+hx1)/(2*s)*rep, 0.0f); glVertex3f(hx1, floor_y, -s);
    glTexCoord2f((s+hx1)/(2*s)*rep, rep);  glVertex3f(hx1, ceil_y, -s);
    glTexCoord2f(0.0f, rep);  glVertex3f(-s, ceil_y, -s);
    glEnd();
    
    glBegin(GL_QUADS);
    glTexCoord2f((s+hx2)/(2*s)*rep, 0.0f); glVertex3f(hx2, floor_y, -s);
    glTexCoord2f(rep, 0.0f); glVertex3f(s, floor_y, -s);
    glTexCoord2f(rep, rep);  glVertex3f(s, ceil_y, -s);
    glTexCoord2f((s+hx2)/(2*s)*rep, rep);  glVertex3f(hx2, ceil_y, -s);
    glEnd();
    
    glBegin(GL_QUADS);
    glTexCoord2f((s+hx1)/(2*s)*rep, (hy-floor_y)/h*rep); glVertex3f(hx1, hy, -s);
    glTexCoord2f((s+hx2)/(2*s)*rep, (hy-floor_y)/h*rep); glVertex3f(hx2, hy, -s);
    glTexCoord2f((s+hx2)/(2*s)*rep, rep);  glVertex3f(hx2, ceil_y, -s);
    glTexCoord2f((s+hx1)/(2*s)*rep, rep);  glVertex3f(hx1, ceil_y, -s);
    glEnd();
    
    glNormal3f(0.0f, 0.0f, -1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(hx1, floor_y, -s);
    glTexCoord2f((s+hx1)/(2*s)*rep, 0.0f); glVertex3f(-s, floor_y, -s);
    glTexCoord2f((s+hx1)/(2*s)*rep, rep);  glVertex3f(-s, ceil_y, -s);
    glTexCoord2f(0.0f, rep);  glVertex3f(hx1, ceil_y, -s);
    
    glTexCoord2f((s+hx2)/(2*s)*rep, 0.0f); glVertex3f(s, floor_y, -s);
    glTexCoord2f(rep, 0.0f); glVertex3f(hx2, floor_y, -s);
    glTexCoord2f(rep, rep);  glVertex3f(hx2, ceil_y, -s);
    glTexCoord2f((s+hx2)/(2*s)*rep, rep);  glVertex3f(s, ceil_y, -s);
    
    glTexCoord2f((s+hx1)/(2*s)*rep, (hy-floor_y)/h*rep); glVertex3f(hx2, hy, -s);
    glTexCoord2f((s+hx2)/(2*s)*rep, (hy-floor_y)/h*rep); glVertex3f(hx1, hy, -s);
    glTexCoord2f((s+hx2)/(2*s)*rep, rep);  glVertex3f(hx1, ceil_y, -s);
    glTexCoord2f((s+hx1)/(2*s)*rep, rep);  glVertex3f(hx2, ceil_y, -s);
    glEnd();
    
    glNormal3f(0.0f, 0.0f, -1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( s, floor_y,  s);
        glTexCoord2f(rep,  0.0f); glVertex3f(-s, floor_y,  s);
        glTexCoord2f(rep,  rep);  glVertex3f(-s, ceil_y,   s);
        glTexCoord2f(0.0f, rep);  glVertex3f( s, ceil_y,   s);
    glEnd();
    
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( s, floor_y, -s);
        glTexCoord2f(rep,  0.0f); glVertex3f( s, floor_y,  s);
        glTexCoord2f(rep,  rep);  glVertex3f( s, ceil_y,   s);
        glTexCoord2f(0.0f, rep);  glVertex3f( s, ceil_y,  -s);
    glEnd();
    
    glNormal3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-s, floor_y,  s);
        glTexCoord2f(rep,  0.0f); glVertex3f(-s, floor_y, -s);
        glTexCoord2f(rep,  rep);  glVertex3f(-s, ceil_y,  -s);
        glTexCoord2f(0.0f, rep);  glVertex3f(-s, ceil_y,   s);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopAttrib();
}

static void setup_and_draw_candle(const App* app, CandlePlacement cp, float intensity) {
    glPushMatrix();
    glTranslatef(cp.x, cp.y, cp.z);
    
    float f_x = 0.0f;
    float f_y = 1.25f;
    float f_z = 0.0f;
    float cur_in = intensity * 0.9f;
    
    glEnable(cp.light_id);
    GLfloat amb[] = { cur_in * 0.2f, cur_in * 0.2f, cur_in * 0.2f, 1.0f };
    GLfloat dif[] = { cur_in * 1.0f, cur_in * 0.95f, cur_in * 0.85f, 1.0f };
    GLfloat spc[] = { cur_in * 0.5f, cur_in * 0.5f, cur_in * 0.5f, 1.0f };
    GLfloat lpos[] = { f_x, f_y, f_z, 1.0f };
    glLightfv(cp.light_id, GL_AMBIENT, amb);
    glLightfv(cp.light_id, GL_DIFFUSE, dif);
    glLightfv(cp.light_id, GL_SPECULAR, spc);
    glLightfv(cp.light_id, GL_POSITION, lpos);
    glLightf(cp.light_id, GL_LINEAR_ATTENUATION, 0.15f);
    
    glPushMatrix();
    glScalef(0.3f, 0.3f, 0.3f);
    if (app->scene.candle_dl != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, app->scene.candle_texture_id);
        glColor3f(1.0f, 1.0f, 1.0f);
        glCallList(app->scene.candle_dl);
        glDisable(GL_TEXTURE_2D);
    } else {
        glDisable(GL_LIGHTING);
        glColor3f(0.9f, 0.8f, 0.6f);
        glBegin(GL_QUAD_STRIP);
        for(int i=0; i<=20; i++){
            float a = i * 0.314f;
            float x = cosf(a) * 0.5f, z = sinf(a) * 0.5f;
            glVertex3f(x, 4.0f, z); glVertex3f(x, 0.0f, z);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
    
    glPopMatrix();
}

// Draw 3D scene
void render_scene(const App* app) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    vec3 pos = app->player.camera.position;
    vec3 front = app->player.camera.front;
    vec3 up = app->player.camera.up;
    vec3 center = {pos.x + front.x, pos.y + front.y, pos.z + front.z};
    math3d_lookAt(pos, center, up);
    
    draw_sky(app);
    
    glEnable(GL_LIGHTING); 
    glEnable(GL_COLOR_MATERIAL);
    GLfloat fgc[] = {0.02f, 0.02f, 0.02f, 1.0f};
    
    if (app->scene.is_fog_enabled) {
        glEnable(GL_FOG); 
        glFogfv(GL_FOG_COLOR, fgc); 
        glFogf(GL_FOG_DENSITY, 0.04f); 
        glFogi(GL_FOG_MODE, GL_EXP2);
    } else {
        glDisable(GL_FOG);
    }
    
    draw_environment(app);
    double t = (double)SDL_GetTicks() / 1000.0;
    
    draw_fire_particles( 13.5f, -2.01f,  13.5f, t, 1.0f);
    draw_fire_particles(-13.5f, -2.01f,  13.5f, t, 1.0f);
    draw_fire_particles( 13.5f, -2.01f, -13.5f, t, 1.0f);
    draw_fire_particles(-13.5f, -2.01f, -13.5f, t, 1.0f);
    
    CandlePlacement cnds[] = {
        { 4, -2.01f, 4, GL_LIGHT0 },
        { -4, -2.01f, 4, GL_LIGHT1 },
        { 4, -2.01f, -4, GL_LIGHT2 },
        { -4, -2.01f, -4, GL_LIGHT3 },
        { 8, -2.01f, 0, GL_LIGHT4 },
        { -8, -2.01f, 0, GL_LIGHT5 }
    };
    for (int i = 0; i < 6; i++) {
        draw_fire_particles(cnds[i].x, cnds[i].y + 1.25f, cnds[i].z, t, 0.15f);
        setup_and_draw_candle(app, cnds[i], app->scene.light_intensity);
    }
    
    if (!app->scene.is_cube_mode) {
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        if (app->scene.floor_texture_id != 0) {
            glBindTexture(GL_TEXTURE_2D, app->scene.floor_texture_id);
        }
        glColor3f(1.0f, 1.0f, 1.0f);
        if (app->scene.table_dl != 0) {
            glTranslatef(0.0f, -1.62f, 0.0f);
            glCallList(app->scene.table_dl);
        } else {
            draw_box(0.0f, 0.0f, 0.0f, 1.5f, 0.1f, 1.5f);
            draw_box(-1.3f, -1.055f, -1.3f, 0.15f, 0.955f, 0.15f);
            draw_box( 1.3f, -1.055f, -1.3f, 0.15f, 0.955f, 0.15f);
            draw_box(-1.3f, -1.055f,  1.3f, 0.15f, 0.955f, 0.15f);
            draw_box( 1.3f, -1.055f,  1.3f, 0.15f, 0.955f, 0.15f);
        }
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
    
    for (int i = 0; i < 27; i++) {
        glPushMatrix();
        glTranslatef(0.0f, 1.58f, 0.0f);
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
        if (app->scene.is_fog_enabled) glEnable(GL_FOG);
        glEnable(GL_LIGHTING); glEnable(GL_DEPTH_TEST);
        glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    }
}

int get_clicked_piece(const App* app, double mx, double my, int win_w, int win_h, int fb_w, int fb_h) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING); glDisable(GL_FOG); glDisable(GL_DITHER);
    glLoadIdentity();
    vec3 pos = app->player.camera.position; vec3 front = app->player.camera.front; vec3 up = app->player.camera.up; 
    vec3 center = {pos.x + front.x, pos.y + front.y, pos.z + front.z};
    math3d_lookAt(pos, center, up);
    for (int i = 0; i < 27; i++) {
        glPushMatrix(); 
        glTranslatef(0.0f, 1.58f, 0.0f);
        glMultMatrixf(app->scene.cube.pieces[i].transform);
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
    unsigned char pxr[3];
    glReadPixels(px, py, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pxr);
    if (app->scene.is_fog_enabled) glEnable(GL_FOG);
    glEnable(GL_LIGHTING); glEnable(GL_DITHER);
    if (pxr[0] > 0 && pxr[0] <= 27) return pxr[0] - 1;
    return -1;
}