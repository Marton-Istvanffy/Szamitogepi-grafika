#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "camera.h"
#include "cube.h"
#include "particles.h"
#include "texture.h"
#include "obj/model.h"

typedef struct {
    Camera camera;
    float player_y;
    float velocity_y;
    bool is_grounded;
} Player;

typedef struct {
    RubikCube cube;
    ParticleSystem particle_sys;
    
    Model lamp_model;
    GLuint metal_texture_id;
    GLuint floor_texture_id;
    GLuint wall_texture_id;
    
    float light_intensity;
    bool is_cube_mode;
    float orbit_distance;
    bool is_scrambled;
    int step_count;
} Scene;

typedef struct {
    int mouse_dx;
    int mouse_dy;
    bool mouse_left_click;
    bool mouse_left_pressed;
    bool mouse_right_pressed;
    const Uint8* keyboard_state;
} Input;

typedef struct {
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double delta_time;
    Uint64 last_time;

    Player player;
    Scene scene;
    Input input;
} App;

void app_init(App* app);
void app_run(App* app);
void app_cleanup(App* app);
int get_clicked_piece(const App* app, double mx, double my, int win_w, int win_h, int fb_w, int fb_h);

#endif