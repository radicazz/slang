#include "app.h"

bool app_create(app_t* app, const char* title, int width, int height) {
    SDL_assert(app != NULL);
    SDL_assert(title != NULL);
    SDL_assert(width > 0);
    SDL_assert(height > 0);

    if (SDL_CreateWindowAndRenderer(title, width, height, 0, &app->window, &app->renderer) == false) {
        return false;
    }

    app->is_running = true;

    app->last_tick_time = SDL_GetTicks();
    app->tick_accumulator = 0;

    return true;
}

void app_destroy(app_t* app) {
    SDL_assert(app != NULL);

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

bool app_should_tick(app_t* app, uint64_t tick_interval) {
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
