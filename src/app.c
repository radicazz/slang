#include "app.h"

static bool app_create_text(app_t* app) {
    SDL_assert(app != NULL);

    if (TTF_Init() == false) {
        return false;
    }

    app->ttf_text_engine = TTF_CreateRendererTextEngine(app->sdl_renderer);
    if (app->ttf_text_engine == NULL) {
        return false;
    }

    app->ttf_font_default = TTF_OpenFont("assets/fonts/Segoe UI.ttf", 16);
    if (app->ttf_font_default == NULL) {
        return false;
    }

    return true;
}

static void app_destroy_text(app_t* app) {
    SDL_assert(app != NULL);

    if (app->ttf_font_default != NULL) {
        TTF_CloseFont(app->ttf_font_default);
    }

    if (app->ttf_text_engine != NULL) {
        TTF_DestroyRendererTextEngine(app->ttf_text_engine);
    }

    TTF_Quit();
}

bool app_create(app_t* app, const char* title, int width, int height) {
    SDL_assert(app != NULL);
    SDL_assert(title != NULL);
    SDL_assert(width > 0);
    SDL_assert(height > 0);

    app->sdl_window = NULL;
    app->sdl_renderer = NULL;

    app->ttf_text_engine = NULL;
    app->ttf_font_default = NULL;

    if (SDL_CreateWindowAndRenderer(title, width, height, 0, &app->sdl_window, &app->sdl_renderer) == false) {
        return false;
    }

    app->is_running = true;

    app->last_tick_time = SDL_GetTicks();
    app->tick_accumulator = 0;

    if (app_create_text(app) == false) {
        return false;
    }

    return true;
}

void app_destroy(app_t* app) {
    SDL_assert(app != NULL);

    app_destroy_text(app);

    app->last_tick_time = 0;
    app->tick_accumulator = 0;

    if (app->sdl_renderer != NULL) {
        SDL_DestroyRenderer(app->sdl_renderer);
        app->sdl_renderer = NULL;
    }

    if (app->sdl_window != NULL) {
        SDL_DestroyWindow(app->sdl_window);
        app->sdl_window = NULL;
    }
}

bool app_process_time(app_t* app, uint64_t tick_interval) {
    SDL_assert(app != NULL);

    uint64_t current_time = SDL_GetTicks();
    uint64_t elapsed_time = current_time - app->last_tick_time;
    app->last_tick_time = current_time;

    app->tick_accumulator += elapsed_time;

    if (app->tick_accumulator >= tick_interval) {
        app->tick_accumulator -= tick_interval;
        return true;
    }

    return false;
}
