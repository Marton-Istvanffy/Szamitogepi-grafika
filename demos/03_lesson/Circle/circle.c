#include "circle.h"
#include <math.h>

void set_circle_data(Circle* circle, double x, double y, double radius, Color color) {
    circle->x = x;
    circle->y = y;
    if (radius > 0.0) {
        circle->radius = radius;
    } else {
        circle->radius = NAN;
    }
    circle->color = color;
}

double calc_circle_area(const Circle* circle) {
    return circle->radius * circle->radius * M_PI;
}

bool is_point_in_circle(const Circle* circle, double px, double py) {
    double dx = px - circle->x;
    double dy = py - circle->y;
    return (dx * dx + dy * dy) <= (circle->radius * circle->radius);
}

void draw_circle_by_steps(SDL_Renderer* renderer, const Circle* c, int steps) {
    SDL_SetRenderDrawColor(renderer, c->color.r, c->color.g, c->color.b, 255);
    double step = 2.0 * M_PI / steps;
    for (int i = 0; i < steps; i++) {
        double a1 = i * step;
        double a2 = (i + 1) * step;
        SDL_RenderDrawLine(renderer,
            (int)(c->x + c->radius * cos(a1)), (int)(c->y + c->radius * sin(a1)),
            (int)(c->x + c->radius * cos(a2)), (int)(c->y + c->radius * sin(a2)));
    }
}

void draw_circle_by_angle(SDL_Renderer* renderer, const Circle* c, double angle_step) {
    SDL_SetRenderDrawColor(renderer, c->color.r, c->color.g, c->color.b, 255);
    for (double a = 0; a < 2.0 * M_PI; a += angle_step) {
        double a2 = a + angle_step;
        if (a2 > 2.0 * M_PI) a2 = 2.0 * M_PI;
        SDL_RenderDrawLine(renderer,
            (int)(c->x + c->radius * cos(a)), (int)(c->y + c->radius * sin(a)),
            (int)(c->x + c->radius * cos(a2)), (int)(c->y + c->radius * sin(a2)));
    }
}

void draw_circle_by_max_length(SDL_Renderer* renderer, const Circle* c, double max_len) {
    double circumference = 2.0 * M_PI * c->radius;
    int steps = (int)ceil(circumference / max_len);
    if (steps < 3) steps = 3;
    draw_circle_by_steps(renderer, c, steps);
}

void draw_cross(SDL_Renderer* renderer, const Circle* c) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, (int)c->x - 10, (int)c->y, (int)c->x + 10, (int)c->y);
    SDL_RenderDrawLine(renderer, (int)c->x, (int)c->y - 10, (int)c->x, (int)c->y + 10);
}