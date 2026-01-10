#ifndef WINDOW_FRAME_H
#define WINDOW_FRAME_H

#include <SDL3/SDL.h>

#include "window.h"
#include "../utils/vector.h"

#define WINDOW_FRAME_HEIGHT 32
#define WINDOW_FRAME_PADDING 8
#define WINDOW_FRAME_BUTTON_SIZE 18
#define WINDOW_FRAME_ICON_INSET 4

/**
 * @brief Stores custom window frame interaction state.
 */
typedef struct {
    bool enabled;
    bool dragging;
    bool use_hit_test;
    bool allow_manual_drag;
    vector2i_t drag_mouse_start;
    vector2i_t drag_window_start;
} window_frame_t;

void window_frame_init(window_frame_t* frame);

/**
 * @brief Toggle the custom frame and disable the native window border.
 *
 * @param window Window to update.
 * @param frame Frame state to update.
 * @param enable true to enable the custom frame.
 * @return true if the window border was updated successfully.
 */
bool window_frame_enable(window_t* window, window_frame_t* frame, bool enable);

/**
 * @brief Handle window-frame-specific input.
 *
 * @param window Window owning the frame.
 * @param frame Frame state to update.
 * @param event SDL event to handle.
 * @return true if the event was consumed by the frame.
 */
bool window_frame_handle_event(window_t* window, window_frame_t* frame, const SDL_Event* event);

/**
 * @brief Render the custom window frame.
 *
 * @param window Window used for rendering.
 * @param frame Frame state to render.
 * @return true if rendering succeeded.
 */
bool window_frame_render(window_t* window, window_frame_t* frame);

#endif  // WINDOW_FRAME_H
