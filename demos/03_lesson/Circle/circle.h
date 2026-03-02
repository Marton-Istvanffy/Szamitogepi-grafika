#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct Color {
    int r;
    int g;
    int b;
} Color;

typedef struct Circle {
    double x;
    double y;
    double radius;
    Color color;
} Circle;

void set_circle_data(Circle* circle, double x, double y, double radius, Color color);
double calc_circle_area(const Circle* circle);
bool is_point_in_circle(const Circle* circle, double px, double py);

void draw_circle_by_steps(SDL_Renderer* renderer, const Circle* c, int steps);
void draw_circle_by_angle(SDL_Renderer* renderer, const Circle* c, double angle_step);
void draw_circle_by_max_length(SDL_Renderer* renderer, const Circle* c, double max_len);
void draw_cross(SDL_Renderer* renderer, const Circle* c);

#endif