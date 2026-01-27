#ifndef SNAKE_MENU_H
#define SNAKE_MENU_H

#include <SDL3/SDL.h>

#include "../snake.h"

typedef struct {
    SDL_FRect panel_rect;
    SDL_FRect button_rect;
    SDL_FRect secondary_button_rect;
    SDL_FRect tertiary_button_rect;
    SDL_FPoint title_pos;
    SDL_FPoint subtitle_pos;
    bool has_subtitle;
    bool has_button;
    bool has_secondary_button;
    bool has_tertiary_button;
} snake_menu_layout_t;

bool snake_menu_get_layout(snake_t* snake, TTF_Text* title_text, TTF_Text* subtitle_text, bool has_subtitle,
                           TTF_Text* button_text, bool has_button, snake_menu_layout_t* out_layout);
bool snake_menu_get_layout_with_secondary_button(snake_t* snake, TTF_Text* title_text, TTF_Text* subtitle_text,
                                                 bool has_subtitle, TTF_Text* primary_button_text, bool has_primary,
                                                 TTF_Text* secondary_button_text, bool has_secondary,
                                                 snake_menu_layout_t* out_layout);
bool snake_menu_get_layout_with_three_buttons(snake_t* snake, TTF_Text* title_text, TTF_Text* subtitle_text,
                                              bool has_subtitle, TTF_Text* primary_button_text, bool has_primary,
                                              TTF_Text* secondary_button_text, bool has_secondary,
                                              TTF_Text* tertiary_button_text, bool has_tertiary,
                                              snake_menu_layout_t* out_layout);

#endif  // SNAKE_MENU_H
