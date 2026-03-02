#include "pong.h"
#include <GL/gl.h>

void init_pong(Pong* pong, int width, int height) {
    pong->width = width;
    pong->height = height;
    pong->left_score = 0;
    pong->right_score = 0;
    init_pad(&(pong->left_pad), 0, height, RED_THEME);
    init_pad(&(pong->right_pad), width - 50, height, GREEN_THEME);
    init_ball(&(pong->ball), width / 2, height / 2);
}

void update_pong(Pong* pong, double time) {
    update_pad(&(pong->left_pad), time);
    update_pad(&(pong->right_pad), time);
    update_ball(&(pong->ball), time);
    bounce_ball(pong);
}

void render_pong(Pong* pong) {
    render_pad(&(pong->left_pad));
    render_pad(&(pong->right_pad));
    render_ball(&(pong->ball));

    // Render score counters (simple squares at the top)
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    for(int i = 0; i < pong->left_score; i++) {
        glVertex2f(10 + i * 15, 10);
        glVertex2f(20 + i * 15, 10);
        glVertex2f(20 + i * 15, 20);
        glVertex2f(10 + i * 15, 20);
    }
    glColor3f(0.0, 1.0, 0.0);
    for(int i = 0; i < pong->right_score; i++) {
        glVertex2f(pong->width - 20 - i * 15, 10);
        glVertex2f(pong->width - 10 - i * 15, 10);
        glVertex2f(pong->width - 10 - i * 15, 20);
        glVertex2f(pong->width - 20 - i * 15, 20);
    }
    glEnd();
}

void set_left_pad_position(Pong* pong, float position) {
    set_pad_position(&(pong->left_pad), position);
}

void set_left_pad_speed(Pong* pong, float speed) {
    set_pad_speed(&(pong->left_pad), speed);
}

void set_right_pad_position(Pong* pong, float position) {
    set_pad_position(&(pong->right_pad), position);
}

void set_right_pad_speed(Pong* pong, float speed) {
    set_pad_speed(&(pong->right_pad), speed);
}

void bounce_ball(Pong* pong) {
    // Bounce off top and bottom walls
    if (pong->ball.y - pong->ball.radius < 0) {
        pong->ball.y = pong->ball.radius;
        pong->ball.speed_y *= -1;
        pong->ball.rotation_speed *= -1.1; // Change rotation on wall hit
    }
    if (pong->ball.y + pong->ball.radius > pong->height) {
        pong->ball.y = pong->height - pong->ball.radius;
        pong->ball.speed_y *= -1;
        pong->ball.rotation_speed *= -1.1;
    }

    // Left pad interaction
    if (pong->ball.x - pong->ball.radius < 50) {
        // Check if pad is actually under the ball
        if (pong->ball.y >= pong->left_pad.y && pong->ball.y <= pong->left_pad.y + pong->left_pad.height) {
            pong->ball.x = pong->ball.radius + 50;
            pong->ball.speed_x *= -1.05; // Increase speed
            pong->ball.rotation_speed += pong->left_pad.speed * 0.5; // Apply pad speed to rotation
        } else if (pong->ball.x < 0) {
            // Ball passed the pad, score for right player
            pong->right_score++;
            init_ball(&(pong->ball), pong->width / 2, pong->height / 2);
        }
    }

    // Right pad interaction
    if (pong->ball.x + pong->ball.radius > pong->width - 50) {
        // Check if pad is actually under the ball
        if (pong->ball.y >= pong->right_pad.y && pong->ball.y <= pong->right_pad.y + pong->right_pad.height) {
            pong->ball.x = pong->width - pong->ball.radius - 50;
            pong->ball.speed_x *= -1.05;
            pong->ball.rotation_speed -= 200;
        } else if (pong->ball.x > pong->width) {
            // Ball passed the pad, score for left player
            pong->left_score++;
            init_ball(&(pong->ball), pong->width / 2, pong->height / 2);
        }
    }
}