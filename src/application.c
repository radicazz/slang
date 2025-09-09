#include "application.h"

static bool text_create(application_t* app) {
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

static void text_destroy(application_t* app) {
    SDL_assert(app != NULL);

    if (app->ttf_font_default != NULL) {
        TTF_CloseFont(app->ttf_font_default);
    }

    if (app->ttf_text_engine != NULL) {
        TTF_DestroyRendererTextEngine(app->ttf_text_engine);
    }

    TTF_Quit();
}

bool application_create(application_t* app, const char* title, int width, int height) {
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
    };

    app->time.frame_first = SDL_GetTicks();
    app->time.frame_last = app->time.frame_first;
    app->time.frame_delta = 0;
    app->time.accumulator = 0;

    if (text_create(app) == false) {
        return false;
    }

    return app->is_running = true;
}

void application_destroy(application_t* app) {
    SDL_assert(app != NULL);

    text_destroy(app);

    app->time.frame_first = 0;
    app->time.frame_last = 0;
    app->time.frame_delta = 0;
    app->time.accumulator = 0;

    if (app->sdl_renderer != NULL) {
        SDL_DestroyRenderer(app->sdl_renderer);
        app->sdl_renderer = NULL;
    }

    if (app->sdl_window != NULL) {
        SDL_DestroyWindow(app->sdl_window);
        app->sdl_window = NULL;
    }
}

bool application_should_update_fixed(application_t* app, const Uint64 tick_interval) {
    SDL_assert(app != NULL);
    SDL_assert(tick_interval > 0);

    const Uint64 time_current = SDL_GetTicks();
    app->time.frame_delta = time_current - app->time.frame_last;
    app->time.frame_last = time_current;

    app->time.accumulator += app->time.frame_delta;
    if (app->time.accumulator >= tick_interval) {
        app->time.accumulator -= tick_interval;
        return true;
    }

    return false;
}
