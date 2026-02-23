#ifndef LINE_H
#define LINE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_SHAPE_COUNT 100
#define PALETTE_SIZE 5
#define PALETTE_HEIGHT 50

typedef struct {
    Uint8 r, g, b, a;
} Color;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point p1;
    Point p2;
    Color color;
} Line;

typedef struct {
    Point p1;
    Point p2;
    Color color;
} RectShape;

void draw_palette(SDL_Renderer *renderer, Color palette[], int window_width);
void draw_line(SDL_Renderer *renderer, Line line);
void draw_rect(SDL_Renderer *renderer, RectShape rect);

#endif