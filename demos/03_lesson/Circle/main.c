#include "circle.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CIRCLE_COUNT 100

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Circle Drawer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Circle circles[MAX_CIRCLE_COUNT];
    int circle_count = 0;
    int dragged_circle = -1;

    Color c0 = {255, 0, 0};
    set_circle_data(&circles[circle_count++], 400, 300, 50, c0);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mx = event.button.x;
                    int my = event.button.y;
                    bool clicked_on_existing = false;

                    for (int i = circle_count - 1; i >= 0; i--) {
                        if (is_point_in_circle(&circles[i], mx, my)) {
                            dragged_circle = i;
                            clicked_on_existing = true;
                            break;
                        }
                    }

                    if (!clicked_on_existing && circle_count < MAX_CIRCLE_COUNT) {
                        Color rnd_col = {rand() % 256, rand() % 256, rand() % 256};
                        double rnd_rad = 20.0 + (rand() % 40);
                        set_circle_data(&circles[circle_count++], mx, my, rnd_rad, rnd_col);
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragged_circle = -1;
                }
            }
            else if (event.type == SDL_MOUSEMOTION) {
                if (dragged_circle != -1) {
                    circles[dragged_circle].x = event.motion.x;
                    circles[dragged_circle].y = event.motion.y;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int mx, my;
        SDL_GetMouseState(&mx, &my);

        for (int i = 0; i < circle_count; i++) {
            draw_circle_by_max_length(renderer, &circles[i], 5.0);

            if (is_point_in_circle(&circles[i], mx, my)) {
                draw_cross(renderer, &circles[i]);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}