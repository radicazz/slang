#ifndef APP_H
#define APP_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define APP_WINDOW_WIDTH 500
#define APP_WINDOW_HEIGHT 500

#define APP_TICK_RATE 8
#define APP_TICK_INTERVAL (1000 / APP_TICK_RATE)

/**
 * @brief Time related data for the application.
 * @note All time values are in milliseconds.
 */
typedef struct {
    Uint64 frame_first;
    Uint64 frame_last;
    Uint64 frame_delta;
    Uint64 accumulator;
} app_timing_t;

/**
 * @brief Represents the window state.
 */
typedef struct {
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;

    TTF_TextEngine* ttf_text_engine;
    TTF_Font* ttf_font_default;

    app_timing_t time;

    bool is_running;
} app_t;

bool app_create(app_t* app, const char* title, int width, int height);
void app_destroy(app_t* app);

/**
 * @brief Determines if it's time for a fixed update based on the tick interval.
 *
 * Since some frames run faster than others, updating your game logic each frame
 * leads to inconsistent behavior. This function helps maintain a consistent
 * update rate at a fixed interval.
 *
 * @param app Pointer to the application state.
 * @param tick_interval The desired fixed interval between ticks in milliseconds.
 * @return true if it's time for an update, false otherwise.
 */
bool app_should_update_fixed(app_t* app, Uint64 tick_interval);

#endif  // APP_H
