#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <GLFW/glfw3.h>
#include "app.h"
#include "render.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    App* app = (App*)glfwGetWindowUserPointer(window);
    if (!app) return;
    
    if (action == GLFW_PRESS) app->keys[key] = true;
    else if (action == GLFW_RELEASE) app->keys[key] = false;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS) app->lighting.intensity += 0.2f;
    if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS) app->lighting.intensity -= 0.2f;
    
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        cube_scramble(&app->cube);
    }
    
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (app->is_cube_mode) {
            if (app->cube.is_dragging) cube_snap_and_bake(&app->cube);
            app->is_cube_mode = false;
            app->first_mouse = true; 
            
            // JAVÍTÁS: Ha fejjel lefelé hagytuk el a forgatást, visszaállítjuk az FPS kamerát talpra!
            float p = fmod(app->camera.pitch, 360.0f);
            if (p < 0.0f) p += 360.0f;
            if (p > 90.0f && p < 270.0f) {
                app->camera.yaw += 180.0f; // Megfordulunk
                app->camera.pitch = 180.0f - app->camera.pitch; // Kiegyenesítjük a nyakat
            }
            app->camera.up.y = 1.0f; // Normál gravitáció iránya
            
            // Biztonsági zár visszakapcsolása az FPS kamerához
            if (app->camera.pitch > 89.0f) app->camera.pitch = 89.0f;
            if (app->camera.pitch < -89.0f) app->camera.pitch = -89.0f;
            
            // Front vektor újraszámolása a stabil kilépéshez
            float rad_yaw = app->camera.yaw * (3.14159f / 180.0f);
            float rad_pitch = app->camera.pitch * (3.14159f / 180.0f);
            app->camera.front.x = cos(rad_yaw) * cos(rad_pitch);
            app->camera.front.y = sin(rad_pitch);
            app->camera.front.z = sin(rad_yaw) * cos(rad_pitch);
            
            // A karaktert visszatesszük a padlóra vagy lebegni, de legalább Y=0 magasságba
            app->player_y = app->camera.position.y - 2.0f;
            if (app->player_y < 0.0f) app->player_y = 0.0f;

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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
            if (action == GLFW_PRESS) {
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
            if (action == GLFW_PRESS) {
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
                if (app->cube.is_dragging) {
                    cube_snap_and_bake(&app->cube);
                }
            }
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    App* app = (App*)glfwGetWindowUserPointer(window);
    
    if (app->first_mouse) { 
        app->last_mouse_x = xpos; 
        app->last_mouse_y = ypos; 
        app->first_mouse = false; 
    }
    
    float xoffset = xpos - app->last_mouse_x;
    float yoffset = app->last_mouse_y - ypos; 
    
    app->last_mouse_x = xpos; 
    app->last_mouse_y = ypos;
    
    if (app->is_cube_mode) {
        if (app->right_mouse_pressed) {
            // JAVÍTÁS: Szabad 360 fokos Arcball Orbitális Kamera!
            app->camera.yaw += -xoffset * app->camera.sensitivity;
            app->camera.pitch += yoffset * app->camera.sensitivity;

            // Szögek normalizálása, hogy megállapítsuk, fejjel lefelé vagyunk-e
            float p = fmod(app->camera.pitch, 360.0f);
            if (p < 0.0f) p += 360.0f;
            
            if (p > 90.0f && p < 270.0f) {
                app->camera.up.y = -1.0f; // Megfordítjuk a világot a kamerának
            } else {
                app->camera.up.y = 1.0f;
            }

            // Kézi, korlátozások nélküli gömb-kalkuláció
            float rad_yaw = app->camera.yaw * (3.14159f / 180.0f);
            float rad_pitch = app->camera.pitch * (3.14159f / 180.0f);
            app->camera.front.x = cos(rad_yaw) * cos(rad_pitch);
            app->camera.front.y = sin(rad_pitch);
            app->camera.front.z = sin(rad_yaw) * cos(rad_pitch);
            
            float len = sqrt(app->camera.front.x*app->camera.front.x + app->camera.front.y*app->camera.front.y + app->camera.front.z*app->camera.front.z);
            app->camera.front.x /= len; app->camera.front.y /= len; app->camera.front.z /= len;

            // Pozíció áthelyezése a kocka (0,0,0) köré, megszüntetve a padló-ütközést
            app->camera.position.x = -app->camera.front.x * app->orbit_distance;
            app->camera.position.y = -app->camera.front.y * app->orbit_distance;
            app->camera.position.z = -app->camera.front.z * app->orbit_distance;
            
        } else if (app->cube.is_dragging) {
            // --- Kocka Réteg Forgatása (Tökéletesen működik fejjel lefelé is!) ---
            vec3 front = app->camera.front;
            vec3 up = app->camera.up; 
            
            vec3 right;
            right.x = front.y * up.z - front.z * up.y;
            right.y = front.z * up.x - front.x * up.z;
            right.z = front.x * up.y - front.y * up.x;
            float rlen = sqrt(right.x*right.x + right.y*right.y + right.z*right.z);
            if(rlen > 0) { right.x /= rlen; right.y /= rlen; right.z /= rlen; }
            
            vec3 true_up;
            true_up.x = right.y * front.z - right.z * front.y;
            true_up.y = right.z * front.x - right.x * front.z;
            true_up.z = right.x * front.y - right.y * front.x;
            
            vec3 w_drag;
            w_drag.x = right.x * xoffset + true_up.x * yoffset;
            w_drag.y = right.y * xoffset + true_up.y * yoffset;
            w_drag.z = right.z * xoffset + true_up.z * yoffset;
            
            vec3 r_axis;
            r_axis.x = -front.y * w_drag.z + front.z * w_drag.y;
            r_axis.y = -front.z * w_drag.x + front.x * w_drag.z;
            r_axis.z = -front.x * w_drag.y + front.y * w_drag.x;

            if (app->cube.drag_axis == -1) {
                if (fabs(xoffset) > 1.0f || fabs(yoffset) > 1.0f) {
                    float ax = fabs(r_axis.x);
                    float ay = fabs(r_axis.y);
                    float az = fabs(r_axis.z);
                    
                    if (ax > ay && ax > az) {
                        app->cube.drag_axis = 0; 
                        app->cube.drag_layer_coord = app->cube.pieces[app->cube.picked_piece_id].transform[12];
                    } else if (ay > ax && ay > az) {
                        app->cube.drag_axis = 1; 
                        app->cube.drag_layer_coord = app->cube.pieces[app->cube.picked_piece_id].transform[13];
                    } else {
                        app->cube.drag_axis = 2; 
                        app->cube.drag_layer_coord = app->cube.pieces[app->cube.picked_piece_id].transform[14];
                    }
                }
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "FPS Rubik's Sanctuary", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    srand((unsigned int)time(NULL));

    App app;
    memset(&app, 0, sizeof(App));
    app.first_mouse = true;
    app.is_cube_mode = false;
    app.right_mouse_pressed = false;
    app.lighting.intensity = 1.0f;
    
    app.player_y = 0.0f;
    app.velocity_y = 0.0f;
    app.is_grounded = true;
    
    camera_init(&app.camera);
    cube_init(&app.cube);

    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    float fov = 45.0f, aspect = 800.0f / 600.0f, zNear = 0.1f, zFar = 100.0f;
    float fH = tan((fov / 360.0f) * 3.14159f) * zNear;
    float fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    glMatrixMode(GL_MODELVIEW);

    app.last_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double current_time = glfwGetTime();
        app.delta_time = current_time - app.last_time;
        app.last_time = current_time;

        glfwPollEvents();
        
        app.is_crouching = app.keys[GLFW_KEY_LEFT_CONTROL] || app.keys[GLFW_KEY_C];
        
        if (!app.is_cube_mode) {
            camera_update_fps(&app.camera, (float)app.delta_time, app.keys, &app.player_y, &app.velocity_y, &app.is_grounded, app.is_crouching);
        }

        render_scene(&app);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}