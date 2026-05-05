#include "snake_options_layout.h"

#include <SDL3/SDL_log.h>

#include "snake_util.h"

static const float k_options_panel_padding = 20.f;
static const float k_options_slider_width = 220.f;
static const float k_options_slider_height = 10.f;
static const float k_options_slider_knob_width = 14.f;
static const float k_options_checkbox_size = 20.f;
static const float k_options_content_gap = 16.f;
static const float k_options_row_gap = 14.f;

bool snake_options_layout_get(snake_t* snake, snake_options_layout_t* out_layout) {
    SDL_assert(snake != NULL);
    SDL_assert(out_layout != NULL);

    vector2i_t screen_size;
    if (snake_get_screen_size(snake, &screen_size) == false) {
        return false;
    }

    vector2i_t title_size;
    if (snake_get_text_size(snake, snake->hud.text_options_title, &title_size, "options title") == false) {
        return false;
    }

    vector2i_t volume_label_size;
    if (snake_get_text_size(snake, snake->hud.text_options_volume_label, &volume_label_size, "volume label") == false) {
        return false;
    }

    vector2i_t volume_value_size;
    if (snake_get_text_size(snake, snake->hud.text_options_volume_value, &volume_value_size, "volume value") == false) {
        return false;
    }

    vector2i_t mute_label_size;
    if (snake_get_text_size(snake, snake->hud.text_options_mute_label, &mute_label_size, "mute label") == false) {
        return false;
    }

    vector2i_t resume_label_size;
    if (snake_get_text_size(snake, snake->hud.text_options_resume_label, &resume_label_size, "resume label") == false) {
        return false;
    }

    vector2i_t resume_value_size;
    if (snake_get_text_size(snake, snake->hud.text_options_resume_value, &resume_value_size, "resume value") == false) {
        return false;
    }

    vector2i_t back_label_size;
    if (snake_get_text_size(snake, snake->hud.text_options_back_button, &back_label_size, "back button") == false) {
        return false;
    }

    const float row_height = SDL_max(k_options_slider_height + 8.f, k_options_checkbox_size);
    const float volume_row_width = (float)volume_label_size.x + k_options_content_gap + k_options_slider_width +
                                   k_options_content_gap + (float)volume_value_size.x;
    const float mute_row_width = (float)mute_label_size.x + k_options_content_gap + k_options_checkbox_size;
    const float resume_row_width = (float)resume_label_size.x + k_options_content_gap + k_options_slider_width +
                                   k_options_content_gap + (float)resume_value_size.x;
    const float back_button_width = (float)back_label_size.x + 56.f;

    float content_width = SDL_max((float)title_size.x, volume_row_width);
    content_width = SDL_max(content_width, mute_row_width);
    content_width = SDL_max(content_width, resume_row_width);
    content_width = SDL_max(content_width, back_button_width);

    const float content_height = (float)title_size.y + k_options_row_gap + row_height + k_options_row_gap + row_height +
                                 k_options_row_gap + row_height + k_options_row_gap + (float)back_label_size.y + 24.f;

    ui_panel_init(&out_layout->panel, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    vector2i_t content_size = {(int)(content_width + 0.5f), (int)(content_height + 0.5f)};
    ui_panel_layout_from_content(&out_layout->panel, &screen_size, &content_size, k_options_panel_padding,
                                 k_options_panel_padding);

    const float center_x = out_layout->panel.rect.x + out_layout->panel.rect.w * 0.5f;
    const float row_left = out_layout->panel.rect.x + k_options_panel_padding;

    float cursor_y = out_layout->panel.rect.y + k_options_panel_padding;
    out_layout->title_pos.x = center_x - (float)title_size.x * 0.5f;
    out_layout->title_pos.y = cursor_y;

    cursor_y += (float)title_size.y + k_options_row_gap;

    const float volume_row_center_y = cursor_y + row_height * 0.5f;
    float cursor_x = row_left;
    out_layout->volume_label_pos.x = cursor_x;
    out_layout->volume_label_pos.y = volume_row_center_y - (float)volume_label_size.y * 0.5f;

    cursor_x += (float)volume_label_size.x + k_options_content_gap;
    ui_slider_init(&out_layout->volume_slider, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0},
                   (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    ui_slider_layout(&out_layout->volume_slider, cursor_x + k_options_slider_width * 0.5f, volume_row_center_y,
                     k_options_slider_width, k_options_slider_height, k_options_slider_knob_width);

    cursor_x += k_options_slider_width + k_options_content_gap;
    out_layout->volume_value_pos.x = cursor_x;
    out_layout->volume_value_pos.y = volume_row_center_y - (float)volume_value_size.y * 0.5f;

    cursor_y += row_height + k_options_row_gap;

    const float mute_row_center_y = cursor_y + row_height * 0.5f;
    cursor_x = row_left;
    out_layout->mute_label_pos.x = cursor_x;
    out_layout->mute_label_pos.y = mute_row_center_y - (float)mute_label_size.y * 0.5f;

    cursor_x += (float)mute_label_size.x + k_options_content_gap + k_options_checkbox_size * 0.5f;
    ui_checkbox_init(&out_layout->mute_checkbox, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0},
                     (SDL_Color){0, 0, 0, 0});
    ui_checkbox_layout(&out_layout->mute_checkbox, cursor_x, mute_row_center_y, k_options_checkbox_size);

    cursor_y += row_height + k_options_row_gap;

    const float resume_row_center_y = cursor_y + row_height * 0.5f;
    cursor_x = row_left;
    out_layout->resume_label_pos.x = cursor_x;
    out_layout->resume_label_pos.y = resume_row_center_y - (float)resume_label_size.y * 0.5f;

    cursor_x += (float)resume_label_size.x + k_options_content_gap;
    ui_slider_int_init(&out_layout->resume_slider, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0},
                       (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0}, CONFIG_RESUME_DELAY_MIN,
                       CONFIG_RESUME_DELAY_MAX);
    ui_slider_int_layout(&out_layout->resume_slider, cursor_x + k_options_slider_width * 0.5f, resume_row_center_y,
                         k_options_slider_width, k_options_slider_height, k_options_slider_knob_width);

    cursor_x += k_options_slider_width + k_options_content_gap;
    out_layout->resume_value_pos.x = cursor_x;
    out_layout->resume_value_pos.y = resume_row_center_y - (float)resume_value_size.y * 0.5f;

    cursor_y += row_height + k_options_row_gap;

    ui_button_init(&out_layout->back_button, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    ui_button_layout_from_label(&out_layout->back_button, &back_label_size, center_x,
                                cursor_y + (float)back_label_size.y * 0.5f, 28.f, 12.f);
    ui_button_get_label_position(&out_layout->back_button, &back_label_size, &out_layout->back_label_pos.x,
                                 &out_layout->back_label_pos.y);

    return true;
}
