#ifndef APP_H
#define APP_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define APP_WINDOW_WIDTH 500
#define APP_WINDOW_HEIGHT 500

/**
 * @brief Represents the window state.
 */
typedef struct {
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    TTF_TextEngine* ttf_text_engine;
    TTF_Font* ttf_font_default;

    bool is_running;

    uint64_t last_tick_time;
    uint64_t tick_accumulator;
} app_t;

bool app_create(app_t* app, const char* title, int width, int height);
void app_destroy(app_t* app);

/**
 * @brief Checks if the application should tick based on the tick interval.
 *
 * Use this to run code at regular intervals.
 *
 * @param app The application instance.
 * @param tick_interval The desired tick interval in milliseconds.
 * @return True if the application should tick, false otherwise.
 */
bool app_process_time(app_t* app, uint64_t tick_interval);

#endif  // APP_H
