#ifndef APP_H
#define APP_H

#include <SDL3/SDL.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;

    bool is_running;

    uint64_t last_tick_time;
    uint64_t tick_accumulator;
} app_t;

bool app_create(app_t* app, const char* title, int width, int height);
void app_destroy(app_t* app);

bool app_should_tick(app_t* app, uint64_t tick_interval);

#endif  // APP_H
