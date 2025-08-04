#include <SDL3/SDL_main.h>

#include "sdl_context.h"

int main(int argc, char* argv[]) {
    sdl_context context;
    if (sdl_context_init(&context) == false) {
        return 1;
    }

    if (sdl_context_create(&context, "seasnake", 640, 480) == false) {
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
    }

    sdl_context_destroy(&context);
    return 0;
}
