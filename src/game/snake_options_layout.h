#ifndef SNAKE_OPTIONS_LAYOUT_H
#define SNAKE_OPTIONS_LAYOUT_H

#include "../snake.h"
#include "../modules/ui.h"

typedef struct {
    ui_panel_t panel;
    ui_slider_t volume_slider;
    ui_checkbox_t mute_checkbox;
    ui_slider_int_t resume_slider;
    ui_button_t back_button;
    SDL_FPoint title_pos;
    SDL_FPoint volume_label_pos;
    SDL_FPoint volume_value_pos;
    SDL_FPoint mute_label_pos;
    SDL_FPoint resume_label_pos;
    SDL_FPoint resume_value_pos;
    SDL_FPoint back_label_pos;
} snake_options_layout_t;

bool snake_options_layout_get(snake_t* snake, snake_options_layout_t* out_layout);

#endif  // SNAKE_OPTIONS_LAYOUT_H
