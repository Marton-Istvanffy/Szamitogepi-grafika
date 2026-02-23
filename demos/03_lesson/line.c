#include <stdlib.h>
#include "line.h"

void draw_palette(SDL_Renderer *renderer, Color palette[], int window_width) {
    int swatch_width = window_width / PALETTE_SIZE;
    
    for (int i = 0; i < PALETTE_SIZE; i++) {
        SDL_Rect swatch = { i * swatch_width, 0, swatch_width, PALETTE_HEIGHT };
        SDL_SetRenderDrawColor(renderer, palette[i].r, palette[i].g, palette[i].b, palette[i].a);
        SDL_RenderFillRect(renderer, &swatch);
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &swatch);
    }
}

void draw_line(SDL_Renderer *renderer, Line line) {
    SDL_SetRenderDrawColor(renderer, line.color.r, line.color.g, line.color.b, line.color.a);
    SDL_RenderDrawLine(renderer, line.p1.x, line.p1.y, line.p2.x, line.p2.y);
}

void draw_rect(SDL_Renderer *renderer, RectShape rectShape) {
    SDL_SetRenderDrawColor(renderer, rectShape.color.r, rectShape.color.g, rectShape.color.b, rectShape.color.a);
    
    SDL_Rect r;
    r.x = rectShape.p1.x < rectShape.p2.x ? rectShape.p1.x : rectShape.p2.x;
    r.y = rectShape.p1.y < rectShape.p2.y ? rectShape.p1.y : rectShape.p2.y;
    r.w = abs(rectShape.p1.x - rectShape.p2.x);
    r.h = abs(rectShape.p1.y - rectShape.p2.y);
    
    SDL_RenderFillRect(renderer, &r);
}