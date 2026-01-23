#include "snake_menu.h"

#include <SDL3/SDL_log.h>

#include "../modules/ui.h"

static const float k_menu_panel_padding = 20.f;
static const float k_menu_text_gap = 16.f;
static const float k_menu_button_padding_x = 28.f;
static const float k_menu_button_padding_y = 12.f;

static bool get_screen_size(snake_t* snake, vector2i_t* out_size) {
    SDL_assert(snake != NULL);
    SDL_assert(out_size != NULL);

    if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &out_size->x, &out_size->y) == false) {
        SDL_Log("Failed to query render output size: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    return true;
}

static bool get_text_size(snake_t* snake, TTF_Text* text, vector2i_t* out_size, const char* label) {
    SDL_assert(snake != NULL);
    SDL_assert(text != NULL);
    SDL_assert(out_size != NULL);
    SDL_assert(label != NULL);

    if (TTF_GetTextSize(text, &out_size->x, &out_size->y) == false) {
        SDL_Log("Failed to measure %s text: %s", label, SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    return true;
}

static void compute_menu_layout(const vector2i_t* screen_size, const vector2i_t* title_size,
                                const vector2i_t* subtitle_size, bool has_subtitle, const vector2i_t* button_label_size,
                                bool has_button, const vector2i_t* secondary_button_size, bool has_secondary,
                                snake_menu_layout_t* out_layout) {
    SDL_assert(screen_size != NULL);
    SDL_assert(title_size != NULL);
    SDL_assert(button_label_size != NULL);
    SDL_assert(out_layout != NULL);

    ui_button_t button;
    ui_button_init(&button, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});

    float button_width = 0.f;
    float button_height = 0.f;
    if (has_button == true && button_label_size != NULL) {
        ui_button_layout_from_label(&button, button_label_size, 0.f, 0.f, k_menu_button_padding_x,
                                    k_menu_button_padding_y);
        button_width = button.rect.w;
        button_height = button.rect.h;
    }

    float secondary_button_width = 0.f;
    float secondary_button_height = 0.f;
    if (has_secondary == true && secondary_button_size != NULL) {
        ui_button_layout_from_label(&button, secondary_button_size, 0.f, 0.f, k_menu_button_padding_x,
                                    k_menu_button_padding_y);
        secondary_button_width = button.rect.w;
        secondary_button_height = button.rect.h;
    }

    float content_width = (float)title_size->x;
    if (has_subtitle == true && subtitle_size != NULL) {
        content_width = SDL_max(content_width, (float)subtitle_size->x);
    }
    if (has_button == true) {
        content_width = SDL_max(content_width, button_width);
    }
    if (has_secondary == true) {
        content_width = SDL_max(content_width, secondary_button_width);
    }

    float content_height = (float)title_size->y;
    if (has_subtitle == true && subtitle_size != NULL) {
        content_height += k_menu_text_gap + (float)subtitle_size->y;
    }
    if (has_button == true) {
        content_height += k_menu_text_gap + button_height;
    }
    if (has_secondary == true) {
        content_height += k_menu_text_gap + secondary_button_height;
    }

    vector2i_t content_size = {(int)(content_width + 0.5f), (int)(content_height + 0.5f)};

    ui_panel_t panel;
    ui_panel_init(&panel, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    ui_panel_layout_from_content(&panel, screen_size, &content_size, k_menu_panel_padding, k_menu_panel_padding);
    out_layout->panel_rect = panel.rect;

    float cursor_y = panel.rect.y + k_menu_panel_padding;
    out_layout->title_pos.x = panel.rect.x + (panel.rect.w - (float)title_size->x) * 0.5f;
    out_layout->title_pos.y = cursor_y;
    cursor_y += (float)title_size->y;

    out_layout->has_subtitle = has_subtitle;
    if (has_subtitle == true && subtitle_size != NULL) {
        cursor_y += k_menu_text_gap;
        out_layout->subtitle_pos.x = panel.rect.x + (panel.rect.w - (float)subtitle_size->x) * 0.5f;
        out_layout->subtitle_pos.y = cursor_y;
        cursor_y += (float)subtitle_size->y;
    }

    out_layout->has_button = has_button;
    if (has_button == true && button_label_size != NULL) {
        cursor_y += k_menu_text_gap;
        const float button_center_x = panel.rect.x + panel.rect.w * 0.5f;
        const float button_center_y = cursor_y + button_height * 0.5f;
        ui_button_layout_from_label(&button, button_label_size, button_center_x, button_center_y,
                                    k_menu_button_padding_x, k_menu_button_padding_y);
        out_layout->button_rect = button.rect;
        cursor_y += button_height;
    } else {
        out_layout->button_rect = (SDL_FRect){0.f, 0.f, 0.f, 0.f};
    }

    out_layout->has_secondary_button = has_secondary;
    if (has_secondary == true && secondary_button_size != NULL) {
        cursor_y += k_menu_text_gap;
        const float button_center_x = panel.rect.x + panel.rect.w * 0.5f;
        const float button_center_y = cursor_y + secondary_button_height * 0.5f;
        ui_button_layout_from_label(&button, secondary_button_size, button_center_x, button_center_y,
                                    k_menu_button_padding_x, k_menu_button_padding_y);
        out_layout->secondary_button_rect = button.rect;
        cursor_y += secondary_button_height;
    } else {
        out_layout->secondary_button_rect = (SDL_FRect){0.f, 0.f, 0.f, 0.f};
    }
}

bool snake_menu_get_layout(snake_t* snake, TTF_Text* title_text, TTF_Text* subtitle_text, bool has_subtitle,
                           TTF_Text* button_text, bool has_button, snake_menu_layout_t* out_layout) {
    SDL_assert(snake != NULL);
    SDL_assert(title_text != NULL);
    SDL_assert(out_layout != NULL);
    if (has_button == true) {
        SDL_assert(button_text != NULL);
    }

    vector2i_t screen_size;
    if (get_screen_size(snake, &screen_size) == false) {
        return false;
    }

    vector2i_t title_size;
    if (get_text_size(snake, title_text, &title_size, "title") == false) {
        return false;
    }

    vector2i_t subtitle_size = {0, 0};
    if (has_subtitle == true) {
        SDL_assert(subtitle_text != NULL);
        if (get_text_size(snake, subtitle_text, &subtitle_size, "subtitle") == false) {
            return false;
        }
    }

    vector2i_t button_size = {0, 0};
    if (has_button == true) {
        if (get_text_size(snake, button_text, &button_size, "button") == false) {
            return false;
        }
    }

    vector2i_t secondary_size = {0, 0};
    compute_menu_layout(&screen_size, &title_size, &subtitle_size, has_subtitle, &button_size, has_button,
                        &secondary_size, false, out_layout);
    return true;
}

bool snake_menu_get_layout_with_secondary_button(snake_t* snake, TTF_Text* title_text, TTF_Text* subtitle_text,
                                                 bool has_subtitle, TTF_Text* primary_button_text, bool has_primary,
                                                 TTF_Text* secondary_button_text, bool has_secondary,
                                                 snake_menu_layout_t* out_layout) {
    SDL_assert(snake != NULL);
    SDL_assert(title_text != NULL);
    SDL_assert(out_layout != NULL);
    if (has_primary == true) {
        SDL_assert(primary_button_text != NULL);
    }
    if (has_secondary == true) {
        SDL_assert(secondary_button_text != NULL);
    }

    vector2i_t screen_size;
    if (get_screen_size(snake, &screen_size) == false) {
        return false;
    }

    vector2i_t title_size;
    if (get_text_size(snake, title_text, &title_size, "title") == false) {
        return false;
    }

    vector2i_t subtitle_size = {0, 0};
    if (has_subtitle == true) {
        SDL_assert(subtitle_text != NULL);
        if (get_text_size(snake, subtitle_text, &subtitle_size, "subtitle") == false) {
            return false;
        }
    }

    vector2i_t primary_size = {0, 0};
    if (has_primary == true) {
        if (get_text_size(snake, primary_button_text, &primary_size, "primary button") == false) {
            return false;
        }
    }

    vector2i_t secondary_size = {0, 0};
    if (has_secondary == true) {
        if (get_text_size(snake, secondary_button_text, &secondary_size, "secondary button") == false) {
            return false;
        }
    }

    compute_menu_layout(&screen_size, &title_size, &subtitle_size, has_subtitle, &primary_size, has_primary,
                        &secondary_size, has_secondary, out_layout);
    return true;
}
