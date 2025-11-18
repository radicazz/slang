#include "window.h"
#include "SDL3/SDL_init.h"

static bool text_create(window_t* window) {
    SDL_assert(window != NULL);

    if (TTF_Init() == false) {
        return false;
    }

    window->ttf_text_engine = TTF_CreateRendererTextEngine(window->sdl_renderer);
    if (window->ttf_text_engine == NULL) {
        return false;
    }

    window->ttf_font_default = TTF_OpenFont("assets/fonts/Segoe UI.ttf", 16);
    if (window->ttf_font_default == NULL) {
        return false;
    }

    return true;
}

static void text_destroy(window_t* window) {
    SDL_assert(window != NULL);

    if (window->ttf_font_default != NULL) {
        TTF_CloseFont(window->ttf_font_default);
    }

    if (window->ttf_text_engine != NULL) {
        TTF_DestroyRendererTextEngine(window->ttf_text_engine);
    }

    TTF_Quit();
}

bool window_create(window_t* window, const char* title, int width, int height) {
    SDL_assert(window != NULL);
    SDL_assert(title != NULL);
    SDL_assert(width > 0);
    SDL_assert(height > 0);

    window->sdl_window = NULL;
    window->sdl_renderer = NULL;

    window->ttf_text_engine = NULL;
    window->ttf_font_default = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        return false;
    }

    if (SDL_CreateWindowAndRenderer(title, width, height, 0, &window->sdl_window, &window->sdl_renderer) == false) {
        return false;
    };

    window->time.frame_first = SDL_GetTicks();
    window->time.frame_last = window->time.frame_first;
    window->time.frame_delta = 0;
    window->time.accumulator = 0;

    if (text_create(window) == false) {
        return false;
    }

    return window->is_running = true;
}

void window_destroy(window_t* window) {
    SDL_assert(window != NULL);

    text_destroy(window);

    window->time.frame_first = 0;
    window->time.frame_last = 0;
    window->time.frame_delta = 0;
    window->time.accumulator = 0;

    if (window->sdl_renderer != NULL) {
        SDL_DestroyRenderer(window->sdl_renderer);
        window->sdl_renderer = NULL;
    }

    if (window->sdl_window != NULL) {
        SDL_DestroyWindow(window->sdl_window);
        window->sdl_window = NULL;
    }

    SDL_Quit();
}

bool window_can_update_fixed(window_t* window, const Uint64 tick_interval) {
    SDL_assert(window != NULL);
    SDL_assert(tick_interval > 0);

    const Uint64 time_current = SDL_GetTicks();
    window->time.frame_delta = time_current - window->time.frame_last;
    window->time.frame_last = time_current;

    window->time.accumulator += window->time.frame_delta;
    if (window->time.accumulator >= tick_interval) {
        window->time.accumulator -= tick_interval;
        return true;
    }

    return false;
}
