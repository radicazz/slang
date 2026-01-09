#ifndef SNAKE_MENU_H
#define SNAKE_MENU_H

#include <SDL3/SDL.h>

#include "../snake.h"

typedef struct {
    SDL_FRect panel_rect;
    SDL_FRect button_rect;
    SDL_FPoint title_pos;
    SDL_FPoint subtitle_pos;
    bool has_subtitle;
} snake_menu_layout_t;

bool snake_menu_get_layout(snake_t* snake, TTF_Text* title_text, TTF_Text* subtitle_text, bool has_subtitle,
                           TTF_Text* button_text, snake_menu_layout_t* out_layout);

#endif  // SNAKE_MENU_H
