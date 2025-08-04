#ifndef SDL_CONTEXT_H
#define SDL_CONTEXT_H

#include <SDL3/SDL.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} sdl_context;

bool sdl_context_init(sdl_context* ctx);

bool sdl_context_create(sdl_context* ctx, const char* title, int width, int height);
void sdl_context_destroy(sdl_context* ctx);

#endif  // SDL_CONTEXT_H
