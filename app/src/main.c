#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GLFW/glfw3.h>
#include "app.h"
#include "render.h"

void print_help_menu() {
    printf("\n--- MAGIC RUBIK SANCTUARY CONTROLS ---\n");
    printf("F1            : Help menu\n");
    printf("WASD          : Move (FPS mode)\n");
    printf("Space / Ctrl  : Jump / Crouch (FPS mode)\n");
    printf("L-Click       : Pick piece / Rotate layer\n");
    printf("R-Click+Drag  : Orbit camera (Cube mode)\n");
    printf("V             : Exit Cube mode\n");
    printf("L             : Scramble cube\n");
    printf("P / O         : Increase / Decrease light intensity\n");
    printf("Esc           : Quit\n");
    printf("--------------------------------------\n\n");
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (!app) return;
    
    if (action == GLFW_PRESS) app->keys[key] = true;
    else if (action == GLFW_RELEASE) app->keys[key] = false;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) print_help_menu();

    // kocka keverése és lépések nullázása
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        if (!app->cube.is_animating) {
            cube_scramble(&app->cube);
            app->step_count = 0;
            app->is_scrambled = true;
            printf("\n=> KOCKA ÖSSZEKEVERVE! Lepesek nullazva.\n");
        }
    }
    
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (app->is_cube_mode) {
            if (app->cube.is_dragging) cube_start_anim(&app->cube);
            app->is_cube_mode = false;
            app->first_mouse = true; 
            
            float p = fmod(app->camera.pitch, 360.0f);
            if (p < 0.0f) p += 360.0f;
            if (p > 90.0f && p < 270.0f) {
                app->camera.yaw += 180.0f;
                app->camera.pitch = 180.0f - app->camera.pitch;
            }
            app->camera.up.y = 1.0f;
            
            if (app->camera.pitch > 89.0f) app->camera.pitch = 89.0f;
            if (app->camera.pitch < -89.0f) app->camera.pitch = -89.0f;
            
            float rad_yaw = app->camera.yaw * (3.14159f / 180.0f);
            float rad_pitch = app->camera.pitch * (3.14159f / 180.0f);
            app->camera.front.x = cos(rad_yaw) * cos(rad_pitch);
            app->camera.front.y = sin(rad_pitch);
            app->camera.front.z = sin(rad_yaw) * cos(rad_pitch);
            
            app->player_y = app->camera.position.y - 2.0f;
            if (app->player_y < 0.0f) app->player_y = 0.0f;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    App* app = (App*)glfwGetWindowUserPointer(window);
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) app->right_mouse_pressed = true;
        else if (action == GLFW_RELEASE) {
            app->right_mouse_pressed = false;
            app->first_mouse = true; 
        }
    }
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        int win_w, win_h, fb_w, fb_h;
        glfwGetWindowSize(window, &win_w, &win_h);
        glfwGetFramebufferSize(window, &fb_w, &fb_h);

        if (!app->is_cube_mode) {
            if (action == GLFW_PRESS && !app->cube.is_animating) {
                int picked = get_clicked_piece(app, -1.0, -1.0, win_w, win_h, fb_w, fb_h);
                if (picked >= 0) {
                    app->is_cube_mode = true; 
                    app->first_mouse = true;
                    
                    app->orbit_distance = sqrt(
                        app->camera.position.x * app->camera.position.x +
                        app->camera.position.y * app->camera.position.y +
                        app->camera.position.z * app->camera.position.z
                    );
                    
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
        } else {
            if (action == GLFW_PRESS && !app->cube.is_animating) {
                double mx, my;
                glfwGetCursorPos(window, &mx, &my);
                int picked = get_clicked_piece(app, mx, my, win_w, win_h, fb_w, fb_h);
                if (picked >= 0) {
                    app->cube.is_dragging = true; 
                    app->cube.drag_axis = -1;
                    app->cube.picked_piece_id = picked;
                    app->cube.drag_angle = 0.0f;
                }
            } else if (action == GLFW_RELEASE) {
                if (app->cube.is_dragging) cube_start_anim(&app->cube);
            }
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (app->first_mouse) { 
        app->last_mouse_x = xpos; app->last_mouse_y = ypos; app->first_mouse = false; 
    }
    float xoffset = xpos - app->last_mouse_x;
    float yoffset = app->last_mouse_y - ypos; 
    app->last_mouse_x = xpos; app->last_mouse_y = ypos;
    
    if (app->is_cube_mode) {
        if (app->right_mouse_pressed) {
            app->camera.yaw += -xoffset * app->camera.sensitivity;
            app->camera.pitch += yoffset * app->camera.sensitivity;

            float p = fmod(app->camera.pitch, 360.0f);
            if (p < 0.0f) p += 360.0f;
            if (p > 90.0f && p < 270.0f) app->camera.up.y = -1.0f; 
            else app->camera.up.y = 1.0f;

            float rad_yaw = app->camera.yaw * (3.14159f / 180.0f);
            float rad_pitch = app->camera.pitch * (3.14159f / 180.0f);
            app->camera.front.x = cos(rad_yaw) * cos(rad_pitch);
            app->camera.front.y = sin(rad_pitch);
            app->camera.front.z = sin(rad_yaw) * cos(rad_pitch);
            
            float len = sqrt(app->camera.front.x*app->camera.front.x + app->camera.front.y*app->camera.front.y + app->camera.front.z*app->camera.front.z);
            app->camera.front.x /= len; app->camera.front.y /= len; app->camera.front.z /= len;

            app->camera.position.x = -app->camera.front.x * app->orbit_distance;
            app->camera.position.y = -app->camera.front.y * app->orbit_distance;
            app->camera.position.z = -app->camera.front.z * app->orbit_distance;
            
        } else if (app->cube.is_dragging && !app->cube.is_animating) {
            vec3 front = app->camera.front;
            vec3 up = app->camera.up; 
            
            vec3 right = { front.y * up.z - front.z * up.y, front.z * up.x - front.x * up.z, front.x * up.y - front.y * up.x };
            float rlen = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
            if(rlen > 0) { right.x /= rlen; right.y /= rlen; right.z /= rlen; }
            
            vec3 true_up = { right.y * front.z - right.z * front.y, right.z * front.x - right.x * front.z, right.x * front.y - right.y * front.x };
            vec3 w_drag = { right.x * xoffset + true_up.x * yoffset, right.y * xoffset + true_up.y * yoffset, right.z * xoffset + true_up.z * yoffset };
            vec3 r_axis = { -front.y * w_drag.z + front.z * w_drag.y, -front.z * w_drag.x + front.x * w_drag.z, -front.x * w_drag.y + front.y * w_drag.x };

            if (app->cube.drag_axis == -1 && (fabs(xoffset) > 1.0f || fabs(yoffset) > 1.0f)) {
                float ax = fabs(r_axis.x), ay = fabs(r_axis.y), az = fabs(r_axis.z);
                if (ax > ay && ax > az) { app->cube.drag_axis = 0; app->cube.drag_layer_coord = app->cube.pieces[app->cube.picked_piece_id].transform[12]; }
                else if (ay > ax && ay > az) { app->cube.drag_axis = 1; app->cube.drag_layer_coord = app->cube.pieces[app->cube.picked_piece_id].transform[13]; }
                else { app->cube.drag_axis = 2; app->cube.drag_layer_coord = app->cube.pieces[app->cube.picked_piece_id].transform[14]; }
            }
            
            if (app->cube.drag_axis != -1) {
                float sensitivity = 0.5f;
                if (app->cube.drag_axis == 0) app->cube.drag_angle += r_axis.x * sensitivity;
                if (app->cube.drag_axis == 1) app->cube.drag_angle += r_axis.y * sensitivity;
                if (app->cube.drag_axis == 2) app->cube.drag_angle += r_axis.z * sensitivity;
            }
        }
    } else {
        camera_process_mouse(&app->camera, xoffset, yoffset);
    }
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Rubik's Sanctuary", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    srand((unsigned int)time(NULL));

    App app;
    memset(&app, 0, sizeof(App));
    app.first_mouse = true;
    app.lighting.intensity = 1.0f;
    app.step_count = 0;
    app.is_scrambled = false; 
    
    camera_init(&app.camera);
    cube_init(&app.cube);
    particles_init(&app.particle_sys);

    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
    
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    float fov = 45.0f, aspect = 800.0f / 600.0f, zNear = 0.1f, zFar = 100.0f;
    float fH = tan((fov / 360.0f) * 3.14159f) * zNear, fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);

    app.last_time = glfwGetTime();
    
    printf("Udv a Magikus Rubik Szentejben! (Nyomd meg az F1-et az iranyitashoz)\n");
    
    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        app.delta_time = current_time - app.last_time;
        app.last_time = current_time;
        glfwPollEvents();
        
        if (app.keys[GLFW_KEY_P]) app.lighting.intensity += 1.5f * app.delta_time; 
        if (app.keys[GLFW_KEY_O]) {
            app.lighting.intensity -= 1.5f * app.delta_time; 
            if (app.lighting.intensity < 0.0f) app.lighting.intensity = 0.0f;
        }
        
        if (!app.is_cube_mode) camera_update_fps(&app.camera, (float)app.delta_time, app.keys, &app.player_y, &app.velocity_y, &app.is_grounded, app.keys[GLFW_KEY_LEFT_CONTROL] || app.keys[GLFW_KEY_C]);
        
        // animáció frissítése és játéklogika
        if (cube_update_anim(&app.cube, (float)app.delta_time)) {
            app.step_count++;
            printf("Lepes: %d\n", app.step_count);
            
            // kirakottság vizsgálata
            if (cube_is_solved(&app.cube)) {
                if (app.is_scrambled) { 
                    printf("\n==========================================\n");
                    printf(" GRATULALOK! SIKERESEN KIRAKTAD A KOCKAT!\n");
                    printf("  Osszesen %d lepesbol sikerult!\n", app.step_count);
                    printf("==========================================\n\n");
                    
                    particles_spawn_fireworks(&app.particle_sys); 
                    
                    app.is_scrambled = false; 
                    app.step_count = 0; 
                }
            } else {
                app.is_scrambled = true;
            }
        }
        
        particles_update(&app.particle_sys, (float)app.delta_time);
        render_scene(&app);
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}