#include "sdl_context.h"

bool sdl_context_init(sdl_context* context) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == false) {
        return false;
    }

    context->window = NULL;
    context->renderer = NULL;

    return true;
}

bool sdl_context_create(sdl_context* context, const char* title, int width, int height) {
    if (SDL_CreateWindowAndRenderer(title, width, height, 0, &context->window,
                                    &context->renderer) == false) {
        return false;
    }

    return true;
}

void sdl_context_destroy(sdl_context* context) {
    if (context->renderer) {
        SDL_DestroyRenderer(context->renderer);
        context->renderer = NULL;
    }

    if (context->window) {
        SDL_DestroyWindow(context->window);
        context->window = NULL;
    }

    SDL_Quit();
}
