#ifndef GAME_UI_H
#define GAME_UI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "utils/vector.h"

typedef struct {
    SDL_FRect rect;
    SDL_Color fill_color;
    SDL_Color border_color;
} game_ui_button_t;

void game_ui_button_init(game_ui_button_t* button, SDL_Color fill_color, SDL_Color border_color);
void game_ui_button_layout_from_label(game_ui_button_t* button, const vector2i_t* label_size, float center_x,
                                      float center_y, float padding_x, float padding_y);
void game_ui_button_get_label_position(const game_ui_button_t* button, const vector2i_t* label_size, float* out_x,
                                       float* out_y);
bool game_ui_button_contains(const game_ui_button_t* button, float x, float y);
bool game_ui_button_render(SDL_Renderer* renderer, const game_ui_button_t* button);

#endif  // GAME_UI_H
