#include "app.h"
#include "ui.h"
#include <SDL2/SDL.h>
#include <math.h>

void input_process(App* app) {
    app->input.mouse_dx = 0;
    app->input.mouse_dy = 0;
    app->input.mouse_left_click = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            app->is_running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) app->is_running = false;
            if (event.key.keysym.sym == SDLK_F1) ui_toggle_help(); 
            
            if (event.key.keysym.sym == SDLK_l && !app->scene.cube.is_animating) {
                cube_scramble(&app->scene.cube);
                app->scene.step_count = 0;
                app->scene.is_scrambled = true;
            }
            
            if (event.key.keysym.sym == SDLK_v && app->scene.is_cube_mode) {
                if (app->scene.cube.is_dragging) cube_start_anim(&app->scene.cube);
                app->scene.is_cube_mode = false;

                float p = fmod(app->player.camera.pitch, 360.0f);
                if (p < 0.0f) p += 360.0f;
                if (p > 90.0f && p < 270.0f) {
                    app->player.camera.yaw += 180.0f;
                    app->player.camera.pitch = 180.0f - app->player.camera.pitch;
                }
                app->player.camera.up.y = 1.0f;
                if (app->player.camera.pitch > 89.0f) app->player.camera.pitch = 89.0f;
                if (app->player.camera.pitch < -89.0f) app->player.camera.pitch = -89.0f;
                
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }
        else if (event.type == SDL_MOUSEMOTION) {
            app->input.mouse_dx = event.motion.xrel;
            app->input.mouse_dy = event.motion.yrel;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                app->input.mouse_left_pressed = true;
                app->input.mouse_left_click = true;
            }
            if (event.button.button == SDL_BUTTON_RIGHT) app->input.mouse_right_pressed = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                app->input.mouse_left_pressed = false;
                if (app->scene.is_cube_mode && app->scene.cube.is_dragging) {
                    cube_start_anim(&app->scene.cube);
                }
            }
            if (event.button.button == SDL_BUTTON_RIGHT) app->input.mouse_right_pressed = false;
        }
    }
    app->input.keyboard_state = SDL_GetKeyboardState(NULL);
}