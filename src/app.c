#include "app.h"

static bool app_create_text(app_t* app) {
    SDL_assert(app != NULL);

    if (TTF_Init() == false) {
        return false;
    }

    app->text_engine = TTF_CreateRendererTextEngine(app->renderer);
    if (app->text_engine == NULL) {
        return false;
    }

    app->default_font = TTF_OpenFont("assets/fonts/Segoe UI.ttf", 16);
    if (app->default_font == NULL) {
        return false;
    }

    return true;
}

static void app_destroy_text(app_t* app) {
    SDL_assert(app != NULL);

    if (app->default_font != NULL) {
        TTF_CloseFont(app->default_font);
    }

    if (app->text_engine != NULL) {
        TTF_DestroyRendererTextEngine(app->text_engine);
    }

    TTF_Quit();
}

bool app_create(app_t* app, const char* title, int width, int height) {
    SDL_assert(app != NULL);
    SDL_assert(title != NULL);
    SDL_assert(width > 0);
    SDL_assert(height > 0);

    app->window = NULL;
    app->renderer = NULL;

    app->text_engine = NULL;
    app->default_font = NULL;

    if (SDL_CreateWindowAndRenderer(title, width, height, 0, &app->window, &app->renderer) == false) {
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

    if (app->renderer != NULL) {
        SDL_DestroyRenderer(app->renderer);
        app->renderer = NULL;
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
        app->window = NULL;
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
