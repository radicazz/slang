#include "window_frame.h"

#include <SDL3/SDL_log.h>

static const SDL_Color k_frame_color = {20, 20, 20, 255};
static const SDL_Color k_frame_border_color = {70, 70, 70, 255};
static const SDL_Color k_frame_close_color = {200, 70, 70, 255};
static const SDL_Color k_frame_close_icon_color = {245, 245, 245, 255};

static const float k_frame_padding = 6.f;
static const float k_frame_button_size = 16.f;
static const float k_frame_close_icon_inset = 4.f;

typedef struct {
    SDL_FRect titlebar_rect;
    SDL_FRect close_rect;
    SDL_FRect border_rect;
} window_frame_layout_t;

static bool window_frame_point_in_rect(const SDL_FRect* rect, float x, float y) {
    if (rect == NULL) {
        return false;
    }

    return x >= rect->x && x <= (rect->x + rect->w) && y >= rect->y && y <= (rect->y + rect->h);
}

static bool window_frame_get_layout(window_t* window, window_frame_layout_t* layout) {
    SDL_assert(window != NULL);
    SDL_assert(layout != NULL);
    SDL_assert(window->sdl_window != NULL);

    int width = 0;
    int height = 0;
    if (SDL_GetWindowSize(window->sdl_window, &width, &height) == false) {
        SDL_Log("Failed to query window size for custom frame: %s", SDL_GetError());
        return false;
    }

    layout->titlebar_rect.x = 0.f;
    layout->titlebar_rect.y = 0.f;
    layout->titlebar_rect.w = (float)width;
    layout->titlebar_rect.h = (float)WINDOW_FRAME_HEIGHT;

    layout->border_rect.x = 0.f;
    layout->border_rect.y = 0.f;
    layout->border_rect.w = (float)width;
    layout->border_rect.h = (float)height;

    layout->close_rect.w = k_frame_button_size;
    layout->close_rect.h = k_frame_button_size;
    layout->close_rect.x = layout->titlebar_rect.w - k_frame_padding - layout->close_rect.w;
    layout->close_rect.y = (layout->titlebar_rect.h - layout->close_rect.h) * 0.5f;

    return true;
}

void window_frame_init(window_frame_t* frame) {
    SDL_assert(frame != NULL);

    frame->enabled = false;
    frame->dragging = false;
    vector2i_set(&frame->drag_mouse_start, 0, 0);
    vector2i_set(&frame->drag_window_start, 0, 0);
}

bool window_frame_enable(window_t* window, window_frame_t* frame, bool enable) {
    SDL_assert(window != NULL);
    SDL_assert(frame != NULL);

    if (window->sdl_window == NULL) {
        SDL_Log("Failed to toggle window frame before window creation");
        return false;
    }

    if (SDL_SetWindowBordered(window->sdl_window, enable == false) == false) {
        SDL_Log("Failed to toggle window border: %s", SDL_GetError());
        return false;
    }

    frame->enabled = enable;
    frame->dragging = false;
    return true;
}

bool window_frame_handle_event(window_t* window, window_frame_t* frame, const SDL_Event* event) {
    SDL_assert(window != NULL);
    SDL_assert(frame != NULL);
    SDL_assert(event != NULL);

    if (frame->enabled == false) {
        return false;
    }

    if (event->type == SDL_EVENT_WINDOW_FOCUS_LOST || event->type == SDL_EVENT_WINDOW_MINIMIZED) {
        frame->dragging = false;
        return false;
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT &&
        event->button.down == true) {
        window_frame_layout_t layout;
        if (window_frame_get_layout(window, &layout) == false) {
            return false;
        }

        if (window_frame_point_in_rect(&layout.close_rect, event->button.x, event->button.y) == true) {
            window->is_running = false;
            return true;
        }

        if (window_frame_point_in_rect(&layout.titlebar_rect, event->button.x, event->button.y) == true) {
            int window_x = 0;
            int window_y = 0;
            if (SDL_GetWindowPosition(window->sdl_window, &window_x, &window_y) == false) {
                SDL_Log("Failed to query window position for drag: %s", SDL_GetError());
                return true;
            }

            frame->dragging = true;
            vector2i_set(&frame->drag_mouse_start, (int)event->button.x, (int)event->button.y);
            vector2i_set(&frame->drag_window_start, window_x, window_y);
            return true;
        }
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == SDL_BUTTON_LEFT) {
        if (frame->dragging == true) {
            frame->dragging = false;
            return true;
        }
    }

    if (event->type == SDL_EVENT_MOUSE_MOTION && frame->dragging == true) {
        int delta_x = (int)event->motion.x - frame->drag_mouse_start.x;
        int delta_y = (int)event->motion.y - frame->drag_mouse_start.y;
        int target_x = frame->drag_window_start.x + delta_x;
        int target_y = frame->drag_window_start.y + delta_y;
        if (SDL_SetWindowPosition(window->sdl_window, target_x, target_y) == false) {
            SDL_Log("Failed to update window position: %s", SDL_GetError());
            frame->dragging = false;
        }
        return true;
    }

    return false;
}

bool window_frame_render(window_t* window, window_frame_t* frame) {
    SDL_assert(window != NULL);
    SDL_assert(frame != NULL);

    if (frame->enabled == false) {
        return true;
    }

    window_frame_layout_t layout;
    if (window_frame_get_layout(window, &layout) == false) {
        return false;
    }

    if (SDL_SetRenderDrawColor(window->sdl_renderer, k_frame_color.r, k_frame_color.g, k_frame_color.b,
                               k_frame_color.a) == false) {
        SDL_Log("Failed to set frame color: %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderFillRect(window->sdl_renderer, &layout.titlebar_rect) == false) {
        SDL_Log("Failed to render frame title bar: %s", SDL_GetError());
        return false;
    }

    if (SDL_SetRenderDrawColor(window->sdl_renderer, k_frame_border_color.r, k_frame_border_color.g,
                               k_frame_border_color.b, k_frame_border_color.a) == false) {
        SDL_Log("Failed to set frame border color: %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderRect(window->sdl_renderer, &layout.border_rect) == false) {
        SDL_Log("Failed to render frame border: %s", SDL_GetError());
        return false;
    }

    if (SDL_SetRenderDrawColor(window->sdl_renderer, k_frame_close_color.r, k_frame_close_color.g,
                               k_frame_close_color.b, k_frame_close_color.a) == false) {
        SDL_Log("Failed to set close button color: %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderFillRect(window->sdl_renderer, &layout.close_rect) == false) {
        SDL_Log("Failed to render close button: %s", SDL_GetError());
        return false;
    }

    if (SDL_SetRenderDrawColor(window->sdl_renderer, k_frame_close_icon_color.r, k_frame_close_icon_color.g,
                               k_frame_close_icon_color.b, k_frame_close_icon_color.a) == false) {
        SDL_Log("Failed to set close icon color: %s", SDL_GetError());
        return false;
    }

    float icon_left = layout.close_rect.x + k_frame_close_icon_inset;
    float icon_right = layout.close_rect.x + layout.close_rect.w - k_frame_close_icon_inset;
    float icon_top = layout.close_rect.y + k_frame_close_icon_inset;
    float icon_bottom = layout.close_rect.y + layout.close_rect.h - k_frame_close_icon_inset;

    if (SDL_RenderLine(window->sdl_renderer, icon_left, icon_top, icon_right, icon_bottom) == false) {
        SDL_Log("Failed to render close icon line: %s", SDL_GetError());
        return false;
    }

    if (SDL_RenderLine(window->sdl_renderer, icon_left, icon_bottom, icon_right, icon_top) == false) {
        SDL_Log("Failed to render close icon line: %s", SDL_GetError());
        return false;
    }

    return true;
}
