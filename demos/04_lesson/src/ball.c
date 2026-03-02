#include "ball.h"
#include <GL/gl.h>
#include <math.h>

void init_ball(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
    ball->radius = 30;
    ball->speed_x = 250;
    ball->speed_y = 250;
    ball->rotation = 0;
    ball->rotation_speed = 100;
}

void update_ball(Ball* ball, double time) {
    ball->x += ball->speed_x * time;
    ball->y += ball->speed_y * time;
    ball->rotation += ball->rotation_speed * time; // Update rotation angle
}

void render_ball(Ball* ball) {
    glPushMatrix();
    glTranslatef(ball->x, ball->y, 0.0);
    glRotatef(ball->rotation, 0, 0, 1);
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.4, 0.1, 0.2);
    glVertex2f(0.3, 0.2);
    
    for (double angle = 0; angle <= 2.0 * M_PI + 0.05; angle += 0.1) {
        glColor3f(0.5 + 0.5 * cos(angle), 0.8, 0.9);
        glVertex2f(cos(angle) * ball->radius, sin(angle) * ball->radius);
    }
    glEnd();
    glPopMatrix();
}