#include "snake_util.h"

#include <SDL3/SDL_log.h>

bool snake_get_screen_size(snake_t* snake, vector2i_t* out_size) {
    SDL_assert(snake != NULL);
    SDL_assert(out_size != NULL);

    if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &out_size->x, &out_size->y) == false) {
        SDL_Log("Failed to query render output size: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    return true;
}

bool snake_get_text_size(snake_t* snake, TTF_Text* text, vector2i_t* out_size, const char* label) {
    SDL_assert(snake != NULL);
    SDL_assert(text != NULL);
    SDL_assert(out_size != NULL);
    SDL_assert(label != NULL);

    if (TTF_GetTextSize(text, &out_size->x, &out_size->y) == false) {
        SDL_Log("Failed to measure %s text: %s", label, SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    return true;
}
