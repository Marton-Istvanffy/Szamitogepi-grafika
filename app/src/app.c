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

void update_logic(App* app) {
    static bool prev_k = false;
    bool curr_k = app->input.keyboard_state[SDL_SCANCODE_K];
    if (curr_k && !prev_k) {
        app->scene.is_fog_enabled = !app->scene.is_fog_enabled;
    }
    prev_k = curr_k;

    if (app->input.mouse_left_click && !app->scene.is_cube_mode && !app->scene.cube.is_animating) {
        int w, h;
        SDL_GetWindowSize(app->window, &w, &h);
        int picked = get_clicked_piece(app, w/2, h/2, w, h, w, h);
        if (picked >= 0) {
            app->scene.is_cube_mode = true;
            app->scene.orbit_distance = sqrt(
                app->player.camera.position.x * app->player.camera.position.x + 
                (app->player.camera.position.y - 1.58f) * (app->player.camera.position.y - 1.58f) + 
                app->player.camera.position.z * app->player.camera.position.z
            );
            SDL_SetRelativeMouseMode(SDL_FALSE); 
        }
    }
    if (!app->scene.is_cube_mode) {
        bool is_crouching = app->input.keyboard_state[SDL_SCANCODE_LCTRL] || app->input.keyboard_state[SDL_SCANCODE_C];
        camera_update_fps(&app->player.camera, (float)app->delta_time, app->input.keyboard_state, &app->player.player_y, &app->player.velocity_y, &app->player.is_grounded, is_crouching);
        camera_process_mouse(&app->player.camera, (float)app->input.mouse_dx, (float)-app->input.mouse_dy);
        
        float px = app->player.camera.position.x;
        float pz = app->player.camera.position.z;
        float pr = 0.5f;

        if (pz >= -15.0f) {
            if (px > 15.0f - pr) px = 15.0f - pr;
            if (px < -15.0f + pr) px = -15.0f + pr;
            if (pz > 15.0f - pr) pz = 15.0f - pr;
            
            if (pz < -15.0f + pr) {
                if (px < -1.5f + pr || px > 1.5f - pr) {
                    pz = -15.0f + pr;
                }
            }
        } else {
            if (pz > -15.0f - pr) {
                if (px < -1.5f + pr || px > 1.5f - pr) {
                    pz = -15.0f - pr;
                }
            }
            if (pz < -49.0f) pz = -49.0f;
            if (px > 49.0f) px = 49.0f;
            if (px < -49.0f) px = -49.0f;
        }

        app->player.camera.position.x = px;
        app->player.camera.position.z = pz;
        
    } else {
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
        if (app->input.mouse_right_pressed) {
            app->player.camera.yaw += app->input.mouse_dx * app->player.camera.sensitivity;
            app->player.camera.pitch -= app->input.mouse_dy * app->player.camera.sensitivity;
            float p = fmod(app->player.camera.pitch, 360.0f);
            if (p < 0.0f) p += 360.0f;
            if (p > 90.0f && p < 270.0f) app->player.camera.up.y = -1.0f; 
            else app->player.camera.up.y = 1.0f;
            float rad_yaw = app->player.camera.yaw * (3.14159f / 180.0f);
            float rad_pitch = app->player.camera.pitch * (3.14159f / 180.0f);
            app->player.camera.front.x = cos(rad_yaw) * cos(rad_pitch);
            app->player.camera.front.y = sin(rad_pitch);
            app->player.camera.front.z = sin(rad_yaw) * cos(rad_pitch);
            float len = sqrt(app->player.camera.front.x*app->player.camera.front.x + app->player.camera.front.y*app->player.camera.front.y + app->player.camera.front.z*app->player.camera.front.z);
            app->player.camera.front.x /= len; app->player.camera.front.y /= len; app->player.camera.front.z /= len;
            app->player.camera.position.x = -app->player.camera.front.x * app->scene.orbit_distance;
            app->player.camera.position.y = 1.58f - app->player.camera.front.y * app->scene.orbit_distance;
            app->player.camera.position.z = -app->player.camera.front.z * app->scene.orbit_distance;
        } else if (app->scene.cube.is_dragging && !app->scene.cube.is_animating) {
            vec3 front = app->player.camera.front;
            vec3 up = app->player.camera.up; 
            vec3 right = { front.y * up.z - front.z * up.y, front.z * up.x - front.x * up.z, front.x * up.y - front.y * up.x };
            float rlen = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
            if(rlen > 0) { right.x /= rlen; right.y /= rlen; right.z /= rlen; }
            vec3 true_up = { right.y * front.z - right.z * front.y, right.z * front.x - right.x * front.z, right.x * front.y - right.y * front.x };
            float m_dx = (float)app->input.mouse_dx;
            float m_dy = (float)-app->input.mouse_dy; 
            vec3 w_drag = { right.x * m_dx + true_up.x * m_dy, right.y * m_dx + true_up.y * m_dy, right.z * m_dx + true_up.z * m_dy };
            vec3 r_axis = { -front.y * w_drag.z + front.z * w_drag.y, -front.z * w_drag.x + front.x * w_drag.z, -front.x * w_drag.y + front.y * w_drag.x };
            if (app->scene.cube.drag_axis == -1 && (fabs(m_dx) > 1.0f || fabs(m_dy) > 1.0f)) {
                float ax = fabs(r_axis.x), ay = fabs(r_axis.y), az = fabs(r_axis.z);
                if (ax > ay && ax > az) { app->scene.cube.drag_axis = 0; app->scene.cube.drag_layer_coord = app->scene.cube.pieces[app->scene.cube.picked_piece_id].transform[12]; }
                else if (ay > ax && ay > az) { app->scene.cube.drag_axis = 1; app->scene.cube.drag_layer_coord = app->scene.cube.pieces[app->scene.cube.picked_piece_id].transform[13]; }
                else { app->scene.cube.drag_axis = 2; app->scene.cube.drag_layer_coord = app->scene.cube.pieces[app->scene.cube.picked_piece_id].transform[14]; }
            }
            if (app->scene.cube.drag_axis != -1) {
                float sensitivity = 0.5f;
                if (app->scene.cube.drag_axis == 0) app->scene.cube.drag_angle += r_axis.x * sensitivity;
                if (app->scene.cube.drag_axis == 1) app->scene.cube.drag_angle += r_axis.y * sensitivity;
                if (app->scene.cube.drag_axis == 2) app->scene.cube.drag_angle += r_axis.z * sensitivity;
            }
        }
    }
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
    particles_update(&app->scene.particle_sys, (float)app->delta_time);
}

void app_init(App* app) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    app->window = SDL_CreateWindow("Rubik's Sanctuary", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);    app->gl_context = SDL_GL_CreateContext(app->window);
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