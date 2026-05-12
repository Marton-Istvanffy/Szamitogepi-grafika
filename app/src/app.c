#include "app.h"
#include "ui.h"
#include "render.h" 
#include "obj/load.h"
#include "obj/transform.h"
#include "obj/draw.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL_opengl.h> 
#include <SDL2/SDL_image.h>

extern void input_process(App* app);
extern void render_scene(const App* app);
extern int get_clicked_piece(const App* app, double mx, double my, int win_w, int win_h, int fb_w, int fb_h);

static void handle_fog_toggle(App* app) {
    static bool prev_k = false;
    bool curr_k = app->input.keyboard_state[SDL_SCANCODE_K];
    if (curr_k && !prev_k) { 
        app->scene.is_fog_enabled = !app->scene.is_fog_enabled;
    }
    prev_k = curr_k;
}

static void try_enter_cube_mode(App* app) {
    if (app->input.mouse_left_click && !app->scene.is_cube_mode && !app->scene.cube.is_animating) {
        int w, h;
        SDL_GetWindowSize(app->window, &w, &h);
        int picked = get_clicked_piece(app, w/2, h/2, w, h, w, h);
        
        if (picked >= 0) { 
            app->scene.is_cube_mode = true; 
            vec3* cam_pos = &app->player.camera.position;
            app->scene.orbit_distance = sqrt(
                cam_pos->x * cam_pos->x + 
                (cam_pos->y - 1.58f) * (cam_pos->y - 1.58f) + 
                cam_pos->z * cam_pos->z
            );
            SDL_SetRelativeMouseMode(SDL_FALSE); 
        }
    }
}

static void resolve_player_collisions(Player* player) {
    float px = player->camera.position.x;
    float pz = player->camera.position.z;
    float pr = 0.5f;

    if (pz >= -15.0f) { 
        if (px > 15.0f - pr) px = 15.0f - pr; 
        if (px < -15.0f + pr) px = -15.0f + pr; 
        if (pz > 15.0f - pr) pz = 15.0f - pr; 
        
        if (pz < -15.0f + pr && (px < -1.5f + pr || px > 1.5f - pr)) {
            pz = -15.0f + pr; 
        }
    } else { 
        if (pz > -15.0f - pr && (px < -1.5f + pr || px > 1.5f - pr)) {
            pz = -15.0f - pr;
        }
        if (pz < -49.0f) pz = -49.0f; 
        if (px > 49.0f) px = 49.0f;   
        if (px < -49.0f) px = -49.0f;
    }

    player->camera.position.x = px;
    player->camera.position.z = pz;
}

static void update_fps_mode(App* app) {
    bool is_crouching = app->input.keyboard_state[SDL_SCANCODE_LCTRL] || app->input.keyboard_state[SDL_SCANCODE_C];
    
    camera_update_fps(&app->player.camera, (float)app->delta_time, app->input.keyboard_state, 
                      &app->player.player_y, &app->player.velocity_y, &app->player.is_grounded, is_crouching);
    camera_process_mouse(&app->player.camera, (float)app->input.mouse_dx, (float)-app->input.mouse_dy);
    
    resolve_player_collisions(&app->player);
}

static void handle_cube_picking(App* app) {
    if (app->input.mouse_left_click && !app->scene.cube.is_animating) {
        int w, h, mx, my;
        SDL_GetWindowSize(app->window, &w, &h);
        SDL_GetMouseState(&mx, &my);
        int picked = get_clicked_piece(app, mx, my, w, h, w, h); 
        if (picked >= 0) {
            app->scene.cube.is_dragging = true; 
            app->scene.cube.drag_axis = -1; 
            app->scene.cube.picked_piece_id = picked;
            app->scene.cube.drag_angle = 0.0f;
        }
    }
}

static void handle_camera_orbit(App* app) {
    Camera* cam = &app->player.camera;
    cam->yaw += app->input.mouse_dx * cam->sensitivity;
    cam->pitch -= app->input.mouse_dy * cam->sensitivity;
    
    float p = fmod(cam->pitch, 360.0f);
    if (p < 0.0f) p += 360.0f;
    cam->up.y = (p > 90.0f && p < 270.0f) ? -1.0f : 1.0f; 
    
    float rad_yaw = cam->yaw * (3.14159f / 180.0f);
    float rad_pitch = cam->pitch * (3.14159f / 180.0f);
    cam->front.x = cos(rad_yaw) * cos(rad_pitch);
    cam->front.y = sin(rad_pitch);
    cam->front.z = sin(rad_yaw) * cos(rad_pitch);
    
    float len = sqrt(cam->front.x*cam->front.x + cam->front.y*cam->front.y + cam->front.z*cam->front.z);
    cam->front.x /= len; cam->front.y /= len; cam->front.z /= len;
    
    cam->position.x = -cam->front.x * app->scene.orbit_distance;
    cam->position.y = 1.58f - cam->front.y * app->scene.orbit_distance;
    cam->position.z = -cam->front.z * app->scene.orbit_distance;
}

static void handle_cube_drag(App* app) {
    Camera* cam = &app->player.camera;
    RubikCube* cube = &app->scene.cube;
    
    vec3 right = { cam->front.y * cam->up.z - cam->front.z * cam->up.y, 
                   cam->front.z * cam->up.x - cam->front.x * cam->up.z, 
                   cam->front.x * cam->up.y - cam->front.y * cam->up.x };
    float rlen = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
    if(rlen > 0) { right.x /= rlen; right.y /= rlen; right.z /= rlen; }
    
    vec3 true_up = { right.y * cam->front.z - right.z * cam->front.y, 
                     right.z * cam->front.x - right.x * cam->front.z, 
                     right.x * cam->front.y - right.y * cam->front.x };
    
    float m_dx = (float)app->input.mouse_dx;
    float m_dy = (float)-app->input.mouse_dy; 
    
    vec3 w_drag = { right.x * m_dx + true_up.x * m_dy, 
                    right.y * m_dx + true_up.y * m_dy, 
                    right.z * m_dx + true_up.z * m_dy };
    vec3 r_axis = { -cam->front.y * w_drag.z + cam->front.z * w_drag.y, 
                    -cam->front.z * w_drag.x + cam->front.x * w_drag.z, 
                    -cam->front.x * w_drag.y + cam->front.y * w_drag.x };
    
    if (cube->drag_axis == -1 && (fabs(m_dx) > 1.0f || fabs(m_dy) > 1.0f)) {
        float ax = fabs(r_axis.x), ay = fabs(r_axis.y), az = fabs(r_axis.z);
        if (ax > ay && ax > az) { cube->drag_axis = 0; cube->drag_layer_coord = cube->pieces[cube->picked_piece_id].transform[12]; }
        else if (ay > ax && ay > az) { cube->drag_axis = 1; cube->drag_layer_coord = cube->pieces[cube->picked_piece_id].transform[13]; }
        else { cube->drag_axis = 2; cube->drag_layer_coord = cube->pieces[cube->picked_piece_id].transform[14]; }
    }
    
    if (cube->drag_axis != -1) {
        float sensitivity = 0.5f;
        if (cube->drag_axis == 0) cube->drag_angle += r_axis.x * sensitivity;
        if (cube->drag_axis == 1) cube->drag_angle += r_axis.y * sensitivity;
        if (cube->drag_axis == 2) cube->drag_angle += r_axis.z * sensitivity;
    }
}

static void update_cube_mode(App* app) {
    handle_cube_picking(app);
    
    if (app->input.mouse_right_pressed) {
        handle_camera_orbit(app);
    } else if (app->scene.cube.is_dragging && !app->scene.cube.is_animating) {
        handle_cube_drag(app);
    }
}

static void check_win_condition(App* app) {
    if (cube_update_anim(&app->scene.cube, (float)app->delta_time)) {
        app->scene.step_count++; 
        if (cube_is_solved(&app->scene.cube)) {
            if (app->scene.is_scrambled) {
                particles_spawn_fireworks(&app->scene.particle_sys); 
                app->scene.is_scrambled = false;
                app->scene.step_count = 0; 
            }
        } else {
            app->scene.is_scrambled = true; 
        }
    }
}

void update_logic(App* app) {
    handle_fog_toggle(app);
    try_enter_cube_mode(app);
    
    if (!app->scene.is_cube_mode) {
        update_fps_mode(app);
    } else {
        update_cube_mode(app);
    }
    
    check_win_condition(app);
    particles_update(&app->scene.particle_sys, (float)app->delta_time); 
}

void app_init(App* app) {
    SDL_Init(SDL_INIT_VIDEO); 
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG); 
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);  
    
    app->window = SDL_CreateWindow("Rubik's Sanctuary", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);    
    app->gl_context = SDL_GL_CreateContext(app->window);
    
    glEnable(GL_DEPTH_TEST); 
    glClearColor(0.02f, 0.02f, 0.02f, 1.0f); 
    
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    float fov = 45.0f, aspect = 800.0f / 600.0f, zNear = 0.1f, zFar = 100.0f; 
    float fH = tan((fov / 360.0f) * 3.14159f) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar); 
    glMatrixMode(GL_MODELVIEW); 
    
    app->is_running = true;
    app->last_time = SDL_GetPerformanceCounter();
    camera_init(&app->player.camera);
    app->player.player_y = 0.0f;
    app->player.velocity_y = 0.0f;
    app->player.is_grounded = true;
    app->scene.light_intensity = 1.0f;
    app->scene.is_cube_mode = false;
    app->scene.is_scrambled = false;
    app->scene.step_count = 0;
    app->scene.is_fog_enabled = true;
    
    app->scene.floor_texture_id = load_texture("assets/textures/padlo.jpg");
    app->scene.wall_texture_id  = load_texture("assets/textures/fal.jpg");
    app->scene.sky_texture_id = load_texture("assets/textures/sky.jpg");
    app->scene.candle_texture_id = load_texture("assets/textures/candle_Albedo.jpg");
    
    app->scene.candle_dl = 0;
    app->scene.table_dl = 0;
    
    if (load_model(&(app->scene.candle_model), "assets/models/candle.obj") == 0) {
        app->scene.candle_dl = glGenLists(1);
        glNewList(app->scene.candle_dl, GL_COMPILE);
        draw_model(&app->scene.candle_model);
        glEndList();
    }
    if (load_model(&(app->scene.table_model), "assets/models/table.obj") == 0) {
        scale_model(&(app->scene.table_model), 1.5f, 1.5f, 1.5f); 
        app->scene.table_dl = glGenLists(1);
        glNewList(app->scene.table_dl, GL_COMPILE);
        draw_model(&app->scene.table_model);
        glEndList();
    }
    
    cube_init(&app->scene.cube); 
    particles_init(&app->scene.particle_sys); 
    ui_init(); 
    SDL_SetRelativeMouseMode(SDL_TRUE); 
}

void app_run(App* app) {
    while (app->is_running) { 
        Uint64 current_time = SDL_GetPerformanceCounter();
        app->delta_time = (double)(current_time - app->last_time) / SDL_GetPerformanceFrequency();
        app->last_time = current_time;
        
        input_process(app);
        
        if (app->input.keyboard_state[SDL_SCANCODE_P]) app->scene.light_intensity += 1.5f * app->delta_time; 
        if (app->input.keyboard_state[SDL_SCANCODE_O]) {
            app->scene.light_intensity -= 1.5f * app->delta_time; 
            if (app->scene.light_intensity < 0.0f) app->scene.light_intensity = 0.0f; 
        }
        
        update_logic(app);
        
        render_scene(app);
        ui_render_hud(app->scene.step_count, cube_is_solved(&app->scene.cube)); 
        
        SDL_GL_SwapWindow(app->window);
    }
}

void app_cleanup(App* app) {
    IMG_Quit(); 
    SDL_GL_DeleteContext(app->gl_context); 
    SDL_DestroyWindow(app->window); 
    SDL_Quit(); 
}