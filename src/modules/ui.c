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
