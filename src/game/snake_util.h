#ifndef SNAKE_UTIL_H
#define SNAKE_UTIL_H

#include <SDL3_ttf/SDL_ttf.h>

#include "../snake.h"
#include "../utils/vector.h"

/**
 * @brief Query the current render output size and store it in out_size.
 *
 * On failure logs the error, sets window.is_running = false and returns false.
 */
bool snake_get_screen_size(snake_t* snake, vector2i_t* out_size);

/**
 * @brief Measure a TTF_Text object and store the result in out_size.
 *
 * @param label Human-readable name used in the error log message.
 * On failure logs the error, sets window.is_running = false and returns false.
 */
bool snake_get_text_size(snake_t* snake, TTF_Text* text, vector2i_t* out_size, const char* label);

#endif  // SNAKE_UTIL_H
