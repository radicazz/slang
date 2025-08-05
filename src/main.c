#include <SDL3/SDL_main.h>

#include "sdl_context.h"

int main(int argc, char* argv[]) {
    sdl_context context;
    if (sdl_context_init(&context) == false) {
        return 1;
    }

    if (sdl_context_create(&context, "csnake", 640, 480) == false) {
        sdl_context_destroy(&context);
        return 1;
    }

    bool is_running = true;
    SDL_Event event;
    while (is_running == true) {
        while (SDL_PollEvent(&event) == true) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
                break;
            }
        }

        SDL_RenderClear(context.renderer);

        SDL_SetRenderDrawColor(context.renderer, 255, 255, 255, 255);
        SDL_FRect rect;
        rect.x = 100.0f;
        rect.y = 100.0f;
        rect.w = 50.0f;
        rect.h = 50.0f;
        SDL_RenderFillRect(context.renderer, &rect);

        SDL_SetRenderDrawColor(context.renderer, 0, 0, 0, 255);
        SDL_RenderPresent(context.renderer);
    }

    sdl_context_destroy(&context);
    return 0;
}
