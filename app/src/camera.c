#include "camera.h"
#include <math.h>
#include <GLFW/glfw3.h>

void camera_init(Camera* cam) {
    cam->position = (vec3){0.0f, 2.0f, 8.0f};
    cam->front = (vec3){0.0f, 0.0f, -1.0f};
    cam->up = (vec3){0.0f, 1.0f, 0.0f};
    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
    cam->speed = 5.0f;
    cam->sensitivity = 0.1f;
}

void camera_update_fps(Camera* cam, float deltaTime, const bool* keys, float* player_y, float* velocity_y, bool* is_grounded, bool is_crouching) {
    float current_speed = is_crouching ? 2.5f : cam->speed;
    float velocity = current_speed * deltaTime;

    vec3 front_horiz = {cam->front.x, 0.0f, cam->front.z};
    float len_f = sqrt(front_horiz.x*front_horiz.x + front_horiz.z*front_horiz.z);
    if(len_f > 0.001f) { front_horiz.x /= len_f; front_horiz.z /= len_f; }

    vec3 right_horiz = {
        cam->front.y * cam->up.z - cam->front.z * cam->up.y, 0.0f,
        cam->front.x * cam->up.y - cam->front.y * cam->up.x
    };
    float len_r = sqrt(right_horiz.x*right_horiz.x + right_horiz.z*right_horiz.z);
    if(len_r > 0.001f) { right_horiz.x /= len_r; right_horiz.z /= len_r; }

    if (keys[GLFW_KEY_W]) { cam->position.x += front_horiz.x * velocity; cam->position.z += front_horiz.z * velocity; }
    if (keys[GLFW_KEY_S]) { cam->position.x -= front_horiz.x * velocity; cam->position.z -= front_horiz.z * velocity; }
    if (keys[GLFW_KEY_A]) { cam->position.x -= right_horiz.x * velocity; cam->position.z -= right_horiz.z * velocity; }
    if (keys[GLFW_KEY_D]) { cam->position.x += right_horiz.x * velocity; cam->position.z += right_horiz.z * velocity; }

    *velocity_y -= 15.0f * deltaTime;
    *player_y += *velocity_y * deltaTime;

    if (*player_y <= 0.0f) {
        *player_y = 0.0f; *velocity_y = 0.0f; *is_grounded = true;
    } else { *is_grounded = false; }

    if (keys[GLFW_KEY_SPACE] && *is_grounded) {
        *velocity_y = 6.0f; *is_grounded = false;
    }

    float target_height = is_crouching ? 1.0f : 2.0f;
    cam->position.y = *player_y + target_height;
}

void camera_process_mouse(Camera* cam, float xoffset, float yoffset) {
    xoffset *= cam->sensitivity; yoffset *= cam->sensitivity;
    cam->yaw += xoffset; cam->pitch += yoffset;

    vec3 front;
    front.x = cos(cam->yaw * (3.14159f / 180.0f)) * cos(cam->pitch * (3.14159f / 180.0f));
    front.y = sin(cam->pitch * (3.14159f / 180.0f));
    front.z = sin(cam->yaw * (3.14159f / 180.0f)) * cos(cam->pitch * (3.14159f / 180.0f));
    
    float len = sqrt(front.x*front.x + front.y*front.y + front.z*front.z);
    cam->front = (vec3){front.x/len, front.y/len, front.z/len};
}