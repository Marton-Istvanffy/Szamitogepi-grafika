#include <stdio.h>
#include <stdbool.h>
#include "line.h"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL hiba: %s\n", SDL_GetError());
        return 1;
    }

    int window_width = 800;
    int window_height = 600;

    SDL_Window *window = SDL_CreateWindow("Rajzprogram (Szakaszok es Teglalapok)",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          window_width, window_height, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Ablak letrehozasi hiba: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Color palette[PALETTE_SIZE] = {
        {255, 0, 0, 255},
        {0, 255, 0, 255},
        {0, 0, 255, 255},
        {255, 255, 0, 255},
        {255, 255, 255, 255}
    };
    
    Color current_color = palette[4]; 

    Line lines[MAX_SHAPE_COUNT];
    int line_count = 0;
    
    RectShape rects[MAX_SHAPE_COUNT];
    int rect_count = 0;

    bool running = true;
    bool is_drawing = false;
    bool draw_rect_mode = false; // true esetén téglalapot rajzol, false esetén szakaszt
    Point p1;
    SDL_Event event;

    printf("--- Hasznalat ---\n");
    printf("Bal egergomb: Ketto kattintas a vegpontok megadasahoz (vagy palettara valasztashoz).\n");
    printf("Space: Valtoztatas a Szakasz es Teglalap rajzolo mod kozott.\n");
    printf("-----------------\n\n");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } 
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    draw_rect_mode = !draw_rect_mode;
                    printf("Rajzolo mod valtva: %s\n", draw_rect_mode ? "Teglalap" : "Szakasz");
                    is_drawing = false;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;
                
                printf("Egerkattintas pozicioja: X: %d, Y: %d\n", x, y);

                if (y < PALETTE_HEIGHT) {
                    int swatch_width = window_width / PALETTE_SIZE;
                    int color_index = x / swatch_width;
                    if (color_index >= 0 && color_index < PALETTE_SIZE) {
                        current_color = palette[color_index];
                        printf("Uj szin kivalasztva: index %d\n", color_index);
                        is_drawing = false;
                    }
                } 
                else {
                    if (!is_drawing) {
                        p1.x = x;
                        p1.y = y;
                        is_drawing = true;
                    } else {
                        if (draw_rect_mode) {
                            if (rect_count < MAX_SHAPE_COUNT) {
                                rects[rect_count].p1 = p1;
                                rects[rect_count].p2 = (Point){x, y};
                                rects[rect_count].color = current_color;
                                rect_count++;
                            } else {
                                printf("Elerte a maximalis teglalap szamot!\n");
                            }
                        } else {
                            if (line_count < MAX_SHAPE_COUNT) {
                                lines[line_count].p1 = p1;
                                lines[line_count].p2 = (Point){x, y};
                                lines[line_count].color = current_color;
                                line_count++;
                            } else {
                                printf("Elerte a maximalis szakasz szamot!\n");
                            }
                        }
                        is_drawing = false;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < rect_count; i++) {
            draw_rect(renderer, rects[i]);
        }

        for (int i = 0; i < line_count; i++) {
            draw_line(renderer, lines[i]);
        }
        if (is_drawing) {
            int mouse_x, mouse_y;
            SDL_GetMouseState(&mouse_x, &mouse_y);
            
            if (draw_rect_mode) {
                RectShape preview_rect = { p1, {mouse_x, mouse_y}, current_color };
                draw_rect(renderer, preview_rect);
            } else {
                Line preview_line = { p1, {mouse_x, mouse_y}, current_color };
                draw_line(renderer, preview_line);
            }
        }

        draw_palette(renderer, palette, window_width);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}