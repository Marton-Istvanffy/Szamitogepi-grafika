#ifndef RENDER_H
#define RENDER_H

#include "app.h"

void render_scene(const App* app);
int get_clicked_piece(const App* app, double mx, double my, int win_w, int win_h, int fb_w, int fb_h);

#endif