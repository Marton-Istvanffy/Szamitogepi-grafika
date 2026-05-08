#include "texture.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

GLuint load_texture(const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Hiba a kep betoltesekor (%s): %s\n", filename, IMG_GetError());
        return 0;
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int mode = (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);

    SDL_FreeSurface(surface);
    return texture_id;
}