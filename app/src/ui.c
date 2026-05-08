#include "ui.h"
#include "texture.h" 
#include <SDL2/SDL_opengl.h>
#include <stdio.h>

static bool show_help = true;
static GLuint help_texture = 0;

void ui_init() {
    help_texture = load_texture("assets/textures/help.png");
    if (help_texture == 0) {
        printf("FIGYELMEZTETES: Nem sikerult betolteni a help.png texturat!\n");
    }
}

void ui_toggle_help() {
    show_help = !show_help;
}

static void enter_2d_mode(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void exit_2d_mode() {
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void ui_render_hud(int step_count, bool is_solved) {
    (void)step_count;
    (void)is_solved;

    enter_2d_mode(800, 600);

    if (show_help && help_texture != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, help_texture);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(50.0f, 50.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(750.0f, 50.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(750.0f, 550.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(50.0f, 550.0f);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
    }

    exit_2d_mode();
}