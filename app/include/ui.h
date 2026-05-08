#ifndef UI_H
#define UI_H
#include <stdbool.h>

void ui_init();
void ui_toggle_help();
void ui_render_hud(int step_count, bool is_solved);

#endif