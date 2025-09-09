#ifndef WINDOW_H
#define WINDOW_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

#define WINDOW_TICK_RATE 8
#define WINDOW_TICK_INTERVAL (1000 / WINDOW_TICK_RATE)

/**
 * @brief Time related data for the window.
 * @note All time values are in milliseconds.
 */
typedef struct {
    Uint64 frame_first;
    Uint64 frame_last;
    Uint64 frame_delta;
    Uint64 accumulator;
} window_timing_t;

/**
 * @brief Represents the window state.
 */
typedef struct {
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;

    TTF_TextEngine* ttf_text_engine;
    TTF_Font* ttf_font_default;

    window_timing_t time;

    bool is_running;
} window_t;

bool window_create(window_t* window, const char* title, int width, int height);
void window_destroy(window_t* window);

/**
 * @brief Determines if it's time for a fixed update based on the tick interval.
 *
 * Since some frames run faster than others, updating your game logic each frame
 * leads to inconsistent behavior. This function helps maintain a consistent
 * update rate at a fixed interval.
 *
 * @param window Pointer to the window state.
 * @param tick_interval The desired fixed interval between ticks in milliseconds.
 * @return true if it's time for an update, false otherwise.
 */
bool window_can_update_fixed(window_t* window, Uint64 tick_interval);

#endif  // WINDOW_H
