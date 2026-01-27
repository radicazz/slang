#include "ui.h"

void ui_button_init(ui_button_t* button, SDL_Color fill_color, SDL_Color border_color) {
    SDL_assert(button != NULL);

    button->rect.x = 0.f;
    button->rect.y = 0.f;
    button->rect.w = 0.f;
    button->rect.h = 0.f;
    button->fill_color = fill_color;
    button->border_color = border_color;
}

void ui_button_layout_from_label(ui_button_t* button, const vector2i_t* label_size, float center_x, float center_y,
                                 float padding_x, float padding_y) {
    SDL_assert(button != NULL);
    SDL_assert(label_size != NULL);

    button->rect.w = (float)label_size->x + padding_x * 2.f;
    button->rect.h = (float)label_size->y + padding_y * 2.f;
    button->rect.x = center_x - button->rect.w * 0.5f;
    button->rect.y = center_y - button->rect.h * 0.5f;
}

void ui_button_get_label_position(const ui_button_t* button, const vector2i_t* label_size, float* out_x, float* out_y) {
    SDL_assert(button != NULL);
    SDL_assert(label_size != NULL);
    SDL_assert(out_x != NULL);
    SDL_assert(out_y != NULL);

    *out_x = button->rect.x + (button->rect.w - (float)label_size->x) * 0.5f;
    *out_y = button->rect.y + (button->rect.h - (float)label_size->y) * 0.5f;
}

bool ui_button_contains(const ui_button_t* button, float x, float y) {
    SDL_assert(button != NULL);
    return x >= button->rect.x && x <= button->rect.x + button->rect.w && y >= button->rect.y &&
           y <= button->rect.y + button->rect.h;
}

bool ui_button_render(SDL_Renderer* renderer, const ui_button_t* button) {
    SDL_assert(renderer != NULL);
    SDL_assert(button != NULL);

    SDL_SetRenderDrawColor(renderer, button->fill_color.r, button->fill_color.g, button->fill_color.b,
                           button->fill_color.a);
    if (SDL_RenderFillRect(renderer, &button->rect) == false) {
        return false;
    }

    SDL_SetRenderDrawColor(renderer, button->border_color.r, button->border_color.g, button->border_color.b,
                           button->border_color.a);
    if (SDL_RenderRect(renderer, &button->rect) == false) {
        return false;
    }

    return true;
}

void ui_panel_init(ui_panel_t* panel, SDL_Color fill_color, SDL_Color border_color) {
    SDL_assert(panel != NULL);

    panel->rect.x = 0.f;
    panel->rect.y = 0.f;
    panel->rect.w = 0.f;
    panel->rect.h = 0.f;
    panel->fill_color = fill_color;
    panel->border_color = border_color;
}

void ui_panel_layout_from_content(ui_panel_t* panel, const vector2i_t* screen_size, const vector2i_t* content_size,
                                  float padding_x, float padding_y) {
    SDL_assert(panel != NULL);
    SDL_assert(screen_size != NULL);
    SDL_assert(content_size != NULL);

    panel->rect.w = (float)content_size->x + padding_x * 2.f;
    panel->rect.h = (float)content_size->y + padding_y * 2.f;

    const float min_width = (float)screen_size->x * 0.75f;
    if (panel->rect.w < min_width) {
        panel->rect.w = min_width;
    }

    panel->rect.x = ((float)screen_size->x - panel->rect.w) * 0.5f;
    panel->rect.y = ((float)screen_size->y - panel->rect.h) * 0.5f;
}

bool ui_panel_render(SDL_Renderer* renderer, const ui_panel_t* panel) {
    SDL_assert(renderer != NULL);
    SDL_assert(panel != NULL);

    SDL_SetRenderDrawColor(renderer, panel->fill_color.r, panel->fill_color.g, panel->fill_color.b,
                           panel->fill_color.a);
    if (SDL_RenderFillRect(renderer, &panel->rect) == false) {
        return false;
    }

    if (panel->border_color.a == 0) {
        return true;
    }

    SDL_SetRenderDrawColor(renderer, panel->border_color.r, panel->border_color.g, panel->border_color.b,
                           panel->border_color.a);
    if (SDL_RenderRect(renderer, &panel->rect) == false) {
        return false;
    }

    return true;
}

void ui_checkbox_init(ui_checkbox_t* checkbox, SDL_Color fill_color, SDL_Color border_color, SDL_Color check_color) {
    SDL_assert(checkbox != NULL);

    checkbox->rect.x = 0.f;
    checkbox->rect.y = 0.f;
    checkbox->rect.w = 0.f;
    checkbox->rect.h = 0.f;
    checkbox->fill_color = fill_color;
    checkbox->border_color = border_color;
    checkbox->check_color = check_color;
}

void ui_checkbox_layout(ui_checkbox_t* checkbox, float center_x, float center_y, float size) {
    SDL_assert(checkbox != NULL);

    checkbox->rect.w = size;
    checkbox->rect.h = size;
    checkbox->rect.x = center_x - size * 0.5f;
    checkbox->rect.y = center_y - size * 0.5f;
}

bool ui_checkbox_contains(const ui_checkbox_t* checkbox, float x, float y) {
    SDL_assert(checkbox != NULL);
    return x >= checkbox->rect.x && x <= checkbox->rect.x + checkbox->rect.w && y >= checkbox->rect.y &&
           y <= checkbox->rect.y + checkbox->rect.h;
}

bool ui_checkbox_render(SDL_Renderer* renderer, const ui_checkbox_t* checkbox, bool is_checked) {
    SDL_assert(renderer != NULL);
    SDL_assert(checkbox != NULL);

    SDL_SetRenderDrawColor(renderer, checkbox->fill_color.r, checkbox->fill_color.g, checkbox->fill_color.b,
                           checkbox->fill_color.a);
    if (SDL_RenderFillRect(renderer, &checkbox->rect) == false) {
        return false;
    }

    SDL_SetRenderDrawColor(renderer, checkbox->border_color.r, checkbox->border_color.g, checkbox->border_color.b,
                           checkbox->border_color.a);
    if (SDL_RenderRect(renderer, &checkbox->rect) == false) {
        return false;
    }

    if (is_checked == false) {
        return true;
    }

    const float min_side = SDL_min(checkbox->rect.w, checkbox->rect.h);
    const float inset = min_side * 0.2f;
    SDL_FRect check_rect = {
        checkbox->rect.x + inset,
        checkbox->rect.y + inset,
        checkbox->rect.w - inset * 2.f,
        checkbox->rect.h - inset * 2.f,
    };

    if (check_rect.w <= 0.f || check_rect.h <= 0.f) {
        return true;
    }

    SDL_SetRenderDrawColor(renderer, checkbox->check_color.r, checkbox->check_color.g, checkbox->check_color.b,
                           checkbox->check_color.a);
    if (SDL_RenderFillRect(renderer, &check_rect) == false) {
        return false;
    }

    return true;
}

void ui_slider_init(ui_slider_t* slider, SDL_Color track_color, SDL_Color fill_color, SDL_Color knob_color,
                    SDL_Color border_color) {
    SDL_assert(slider != NULL);

    slider->track_rect = (SDL_FRect){0.f, 0.f, 0.f, 0.f};
    slider->knob_rect = (SDL_FRect){0.f, 0.f, 0.f, 0.f};
    slider->track_color = track_color;
    slider->fill_color = fill_color;
    slider->knob_color = knob_color;
    slider->border_color = border_color;
}

void ui_slider_layout(ui_slider_t* slider, float center_x, float center_y, float width, float height, float knob_width) {
    SDL_assert(slider != NULL);

    slider->track_rect.w = width;
    slider->track_rect.h = height;
    slider->track_rect.x = center_x - width * 0.5f;
    slider->track_rect.y = center_y - height * 0.5f;

    slider->knob_rect.w = knob_width;
    slider->knob_rect.h = height + 8.f;
    slider->knob_rect.y = center_y - slider->knob_rect.h * 0.5f;
}

bool ui_slider_contains(const ui_slider_t* slider, float x, float y) {
    SDL_assert(slider != NULL);

    const float min_x = slider->track_rect.x - slider->knob_rect.w * 0.5f;
    const float max_x = slider->track_rect.x + slider->track_rect.w + slider->knob_rect.w * 0.5f;
    const float min_y = SDL_min(slider->track_rect.y, slider->knob_rect.y);
    const float max_y = SDL_max(slider->track_rect.y + slider->track_rect.h,
                                slider->knob_rect.y + slider->knob_rect.h);
    return x >= min_x && x <= max_x && y >= min_y && y <= max_y;
}

float ui_slider_get_value(const ui_slider_t* slider, float x) {
    SDL_assert(slider != NULL);

    const float start = slider->track_rect.x;
    const float end = slider->track_rect.x + slider->track_rect.w;
    if (end <= start) {
        return 0.0f;
    }

    float value = (x - start) / (end - start);
    if (value < 0.0f) {
        value = 0.0f;
    } else if (value > 1.0f) {
        value = 1.0f;
    }
    return value;
}

bool ui_slider_render(SDL_Renderer* renderer, const ui_slider_t* slider, float value) {
    SDL_assert(renderer != NULL);
    SDL_assert(slider != NULL);

    ui_slider_t mutable_slider = *slider;

    if (value < 0.0f) {
        value = 0.0f;
    } else if (value > 1.0f) {
        value = 1.0f;
    }

    SDL_SetRenderDrawColor(renderer, slider->track_color.r, slider->track_color.g, slider->track_color.b,
                           slider->track_color.a);
    if (SDL_RenderFillRect(renderer, &slider->track_rect) == false) {
        return false;
    }

    SDL_SetRenderDrawColor(renderer, slider->fill_color.r, slider->fill_color.g, slider->fill_color.b,
                           slider->fill_color.a);
    SDL_FRect fill_rect = slider->track_rect;
    fill_rect.w = slider->track_rect.w * value;
    if (SDL_RenderFillRect(renderer, &fill_rect) == false) {
        return false;
    }

    if (slider->border_color.a > 0) {
        SDL_SetRenderDrawColor(renderer, slider->border_color.r, slider->border_color.g, slider->border_color.b,
                               slider->border_color.a);
        if (SDL_RenderRect(renderer, &slider->track_rect) == false) {
            return false;
        }
    }

    mutable_slider.knob_rect.x =
        slider->track_rect.x + slider->track_rect.w * value - slider->knob_rect.w * 0.5f;
    SDL_SetRenderDrawColor(renderer, slider->knob_color.r, slider->knob_color.g, slider->knob_color.b,
                           slider->knob_color.a);
    if (SDL_RenderFillRect(renderer, &mutable_slider.knob_rect) == false) {
        return false;
    }

    if (slider->border_color.a > 0) {
        SDL_SetRenderDrawColor(renderer, slider->border_color.r, slider->border_color.g, slider->border_color.b,
                               slider->border_color.a);
        if (SDL_RenderRect(renderer, &mutable_slider.knob_rect) == false) {
            return false;
        }
    }

    return true;
}

void ui_slider_int_init(ui_slider_int_t* slider, SDL_Color track_color, SDL_Color fill_color, SDL_Color knob_color,
                        SDL_Color border_color, int min_value, int max_value) {
    SDL_assert(slider != NULL);

    ui_slider_init(&slider->slider, track_color, fill_color, knob_color, border_color);
    slider->min_value = min_value;
    slider->max_value = max_value;
}

void ui_slider_int_layout(ui_slider_int_t* slider, float center_x, float center_y, float width, float height,
                          float knob_width) {
    SDL_assert(slider != NULL);

    ui_slider_layout(&slider->slider, center_x, center_y, width, height, knob_width);
}

bool ui_slider_int_contains(const ui_slider_int_t* slider, float x, float y) {
    SDL_assert(slider != NULL);

    return ui_slider_contains(&slider->slider, x, y);
}

int ui_slider_int_get_value(const ui_slider_int_t* slider, float x) {
    SDL_assert(slider != NULL);

    const int range = slider->max_value - slider->min_value;
    if (range <= 0) {
        return slider->min_value;
    }

    const float value = ui_slider_get_value(&slider->slider, x);
    return slider->min_value + (int)((value * (float)range) + 0.5f);
}

bool ui_slider_int_render(SDL_Renderer* renderer, const ui_slider_int_t* slider, int value) {
    SDL_assert(renderer != NULL);
    SDL_assert(slider != NULL);

    const int range = slider->max_value - slider->min_value;
    if (range <= 0) {
        return ui_slider_render(renderer, &slider->slider, 0.0f);
    }

    float normalized = (float)(value - slider->min_value) / (float)range;
    if (normalized < 0.0f) {
        normalized = 0.0f;
    } else if (normalized > 1.0f) {
        normalized = 1.0f;
    }

    return ui_slider_render(renderer, &slider->slider, normalized);
}
