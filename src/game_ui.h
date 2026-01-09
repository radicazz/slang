#ifndef GAME_UI_H
#define GAME_UI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "utils/vector.h"

typedef struct {
    SDL_FRect rect;
    SDL_Color fill_color;
    SDL_Color border_color;
} game_ui_button_t;

/**
 * @brief Initialize a button with colors and zeroed geometry.
 *
 * @param button Button to initialize.
 * @param fill_color Fill color for the button body.
 * @param border_color Border color for the button outline.
 */
void game_ui_button_init(game_ui_button_t* button, SDL_Color fill_color, SDL_Color border_color);

/**
 * @brief Size and position a button based on a text label size.
 *
 * @param button Button to size and position.
 * @param label_size Size of the label text (pixels).
 * @param center_x Desired center X in pixels.
 * @param center_y Desired center Y in pixels.
 * @param padding_x Horizontal padding around the label.
 * @param padding_y Vertical padding around the label.
 */
void game_ui_button_layout_from_label(game_ui_button_t* button, const vector2i_t* label_size, float center_x,
                                      float center_y, float padding_x, float padding_y);

/**
 * @brief Compute the top-left position to render a label centered inside a button.
 *
 * @param button Button to use for alignment.
 * @param label_size Size of the label text (pixels).
 * @param out_x Output X position for the label.
 * @param out_y Output Y position for the label.
 */
void game_ui_button_get_label_position(const game_ui_button_t* button, const vector2i_t* label_size, float* out_x,
                                       float* out_y);

/**
 * @brief Test whether a point lies inside a button's rectangle.
 *
 * @param button Button to test.
 * @param x Point X coordinate.
 * @param y Point Y coordinate.
 * @return true if the point is inside the button rectangle, false otherwise.
 */
bool game_ui_button_contains(const game_ui_button_t* button, float x, float y);

/**
 * @brief Render the button body and border.
 *
 * @param renderer SDL renderer to draw with.
 * @param button Button to draw.
 * @return true if rendering succeeded, false otherwise.
 */
bool game_ui_button_render(SDL_Renderer* renderer, const game_ui_button_t* button);

#endif  // GAME_UI_H
