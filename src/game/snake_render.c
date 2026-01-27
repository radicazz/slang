#include "snake_render.h"

#include <SDL3/SDL_log.h>

#include "snake_menu.h"
#include "../modules/ui.h"

static const SDL_Color k_color_menu_overlay = {0, 0, 0, 160};
static const SDL_Color k_color_menu_panel = {25, 25, 25, 220};
static const SDL_Color k_color_menu_panel_border = {80, 80, 80, 255};
static const SDL_Color k_color_menu_button = {220, 220, 220, 255};
static const SDL_Color k_color_menu_button_border = {180, 180, 180, 255};
static const SDL_Color k_color_menu_checkbox = {30, 30, 30, 255};
static const SDL_Color k_color_menu_checkbox_border = {180, 180, 180, 255};
static const SDL_Color k_color_menu_checkbox_check = {220, 220, 220, 255};
static const SDL_Color k_color_menu_slider_track = {40, 40, 40, 255};
static const SDL_Color k_color_menu_slider_fill = {100, 200, 120, 255};
static const SDL_Color k_color_menu_slider_knob = {230, 230, 230, 255};

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

void snake_render_frame(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->window.sdl_window != NULL);
    SDL_assert(snake->window.sdl_renderer != NULL);

    if (SDL_RenderClear(snake->window.sdl_renderer) == false) {
        SDL_Log("Failed to clear renderer: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    // Loop through all the cells and render them based on their color.
    for (int x = 0; x < SNAKE_GRID_X; ++x) {
        for (int y = 0; y < SNAKE_GRID_Y; ++y) {
            const snake_cell_t* const cell = &snake->cells[x][y];

            // TODO: Optimize the use of SDL_SetRenderDrawColor by rendering all tiles of the same color at once.
            SDL_SetRenderDrawColor(snake->window.sdl_renderer, cell->render_color.r, cell->render_color.g,
                                   cell->render_color.b, cell->render_color.a);

            SDL_FRect rect;
            rect.x = (float)(cell->position.x * SNAKE_CELL_SIZE);
            rect.y = (float)(cell->position.y * SNAKE_CELL_SIZE);
            rect.w = rect.h = (float)SNAKE_CELL_SIZE;

            SDL_RenderFillRect(snake->window.sdl_renderer, &rect);
        }
    }

    vector2i_t screen_size;
    if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &screen_size.x, &screen_size.y) == false) {
        SDL_Log("Failed to query render output size: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    vector2i_t text_size;
    if (TTF_GetTextSize(snake->hud.text_score, &text_size.x, &text_size.y) == false) {
        SDL_Log("Failed to measure score text: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    // Render the score text at the top center of the screen.
    if (TTF_DrawRendererText(snake->hud.text_score, (float)(screen_size.x - text_size.x) * 0.5f, 10.f) == false) {
        SDL_Log("Failed to render score text: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    if (snake->state == SNAKE_STATE_PAUSED || snake->state == SNAKE_STATE_START ||
        snake->state == SNAKE_STATE_RESUMING || snake->state == SNAKE_STATE_GAME_OVER) {
        TTF_Text* title_text = NULL;
        TTF_Text* subtitle_text = NULL;
        bool has_subtitle = false;
        TTF_Text* button_text = NULL;
        bool has_button = false;

        if (snake->state == SNAKE_STATE_PAUSED) {
            title_text = snake->hud.text_pause;
            button_text = snake->hud.text_resume;
            has_button = true;
        } else if (snake->state == SNAKE_STATE_START) {
            title_text = snake->hud.text_start_title;
            subtitle_text = snake->hud.text_start_high_score;
            has_subtitle = true;
            button_text = snake->hud.text_start_button;
            has_button = true;
        } else if (snake->state == SNAKE_STATE_RESUMING) {
            title_text = snake->hud.text_resume_title;
            subtitle_text = NULL;
            has_subtitle = false;
        } else {
            title_text = snake->hud.text_game_over_title;
            subtitle_text = snake->hud.text_game_over_score;
            has_subtitle = true;
            button_text = snake->hud.text_restart_button;
            has_button = true;
        }

        snake_menu_layout_t layout;
        if (snake->state == SNAKE_STATE_PAUSED) {
            if (snake_menu_get_layout_with_three_buttons(snake, title_text, subtitle_text, has_subtitle, button_text,
                                                         has_button, snake->hud.text_options_button, true,
                                                         snake->hud.text_exit_button, true, &layout) == false) {
                return;
            }
        } else if (snake->state == SNAKE_STATE_START) {
            if (snake_menu_get_layout_with_secondary_button(snake, title_text, subtitle_text, has_subtitle, button_text,
                                                            has_button, snake->hud.text_options_button, true, &layout) ==
                false) {
                return;
            }
        } else {
            if (snake_menu_get_layout(snake, title_text, subtitle_text, has_subtitle, button_text, has_button, &layout) ==
                false) {
                return;
            }
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_BLEND) == false) {
            SDL_Log("Failed to set blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_menu_overlay.r, k_color_menu_overlay.g,
                               k_color_menu_overlay.b, k_color_menu_overlay.a);

        SDL_FRect overlay_rect = {0.f, 0.f, (float)screen_size.x, (float)screen_size.y};
        SDL_RenderFillRect(snake->window.sdl_renderer, &overlay_rect);

        ui_panel_t panel;
        ui_panel_init(&panel, k_color_menu_panel, k_color_menu_panel_border);
        panel.rect = layout.panel_rect;
        if (ui_panel_render(snake->window.sdl_renderer, &panel) == false) {
            SDL_Log("Failed to render menu panel: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        ui_button_t button;
        if (layout.has_button == true) {
            ui_button_init(&button, k_color_menu_button, k_color_menu_button_border);
            button.rect = layout.button_rect;
            if (ui_button_render(snake->window.sdl_renderer, &button) == false) {
                SDL_Log("Failed to render menu button: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        }

        if (TTF_DrawRendererText(title_text, layout.title_pos.x, layout.title_pos.y) == false) {
            SDL_Log("Failed to render menu title text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (snake->state == SNAKE_STATE_RESUMING && snake->hud.text_resume_countdown_texture != NULL) {
            vector2i_t screen_size;
            if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &screen_size.x, &screen_size.y) == false) {
                SDL_Log("Failed to query render output size: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
            float countdown_x = (float)(screen_size.x - snake->hud.text_resume_countdown_size.x) * 0.5f;
            float countdown_y = layout.title_pos.y + 60.f;
            SDL_FRect dst = {countdown_x, countdown_y,
                             (float)snake->hud.text_resume_countdown_size.x,
                             (float)snake->hud.text_resume_countdown_size.y};
            if (SDL_RenderTexture(snake->window.sdl_renderer, snake->hud.text_resume_countdown_texture, NULL, &dst) ==
                false) {
                SDL_Log("Failed to render resume countdown texture: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        } else if (layout.has_subtitle == true) {
            if (TTF_DrawRendererText(subtitle_text, layout.subtitle_pos.x, layout.subtitle_pos.y) == false) {
                SDL_Log("Failed to render menu subtitle text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        }

        if (layout.has_button == true) {
            vector2i_t button_text_size;
            if (get_text_size(snake, button_text, &button_text_size, "menu button") == false) {
                return;
            }

            float button_text_x = 0.f;
            float button_text_y = 0.f;
            ui_button_get_label_position(&button, &button_text_size, &button_text_x, &button_text_y);
            if (TTF_DrawRendererText(button_text, button_text_x, button_text_y) == false) {
                SDL_Log("Failed to render menu button text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        }

        if (snake->state == SNAKE_STATE_PAUSED || snake->state == SNAKE_STATE_START) {
            vector2i_t options_text_size;
            if (get_text_size(snake, snake->hud.text_options_button, &options_text_size, "options button") == false) {
                return;
            }

            ui_button_t options_button;
            ui_button_init(&options_button, k_color_menu_button, k_color_menu_button_border);
            options_button.rect = layout.secondary_button_rect;
            if (ui_button_render(snake->window.sdl_renderer, &options_button) == false) {
                SDL_Log("Failed to render options button: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }

            float options_text_x = 0.f;
            float options_text_y = 0.f;
            ui_button_get_label_position(&options_button, &options_text_size, &options_text_x, &options_text_y);
            if (TTF_DrawRendererText(snake->hud.text_options_button, options_text_x, options_text_y) == false) {
                SDL_Log("Failed to render options button text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        }

        if (snake->state == SNAKE_STATE_PAUSED) {
            vector2i_t exit_text_size;
            if (get_text_size(snake, snake->hud.text_exit_button, &exit_text_size, "exit button") == false) {
                return;
            }

            ui_button_t exit_button;
            ui_button_init(&exit_button, k_color_menu_button, k_color_menu_button_border);
            exit_button.rect = layout.tertiary_button_rect;
            if (ui_button_render(snake->window.sdl_renderer, &exit_button) == false) {
                SDL_Log("Failed to render exit button: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }

            float exit_text_x = 0.f;
            float exit_text_y = 0.f;
            ui_button_get_label_position(&exit_button, &exit_text_size, &exit_text_x, &exit_text_y);
            if (TTF_DrawRendererText(snake->hud.text_exit_button, exit_text_x, exit_text_y) == false) {
                SDL_Log("Failed to render exit button text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_NONE) == false) {
            SDL_Log("Failed to reset blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }
    }

    if (snake->state == SNAKE_STATE_OPTIONS) {
        vector2i_t screen_size_options;
        if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &screen_size_options.x,
                                           &screen_size_options.y) == false) {
            SDL_Log("Failed to query render output size: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_BLEND) == false) {
            SDL_Log("Failed to set blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_menu_overlay.r, k_color_menu_overlay.g,
                               k_color_menu_overlay.b, k_color_menu_overlay.a);
        SDL_FRect overlay_rect = {0.f, 0.f, (float)screen_size_options.x, (float)screen_size_options.y};
        SDL_RenderFillRect(snake->window.sdl_renderer, &overlay_rect);

        vector2i_t title_size;
        if (get_text_size(snake, snake->hud.text_options_title, &title_size, "options title") == false) {
            return;
        }

        vector2i_t volume_label_size;
        if (get_text_size(snake, snake->hud.text_options_volume_label, &volume_label_size, "volume label") == false) {
            return;
        }

        vector2i_t volume_value_size;
        if (get_text_size(snake, snake->hud.text_options_volume_value, &volume_value_size, "volume value") == false) {
            return;
        }

        vector2i_t mute_label_size;
        if (get_text_size(snake, snake->hud.text_options_mute_label, &mute_label_size, "mute label") == false) {
            return;
        }

        vector2i_t resume_label_size;
        if (get_text_size(snake, snake->hud.text_options_resume_label, &resume_label_size, "resume label") == false) {
            return;
        }

        vector2i_t resume_value_size;
        if (get_text_size(snake, snake->hud.text_options_resume_value, &resume_value_size, "resume value") == false) {
            return;
        }

        vector2i_t back_label_size;
        if (get_text_size(snake, snake->hud.text_options_back_button, &back_label_size, "back button") == false) {
            return;
        }

        const float slider_width = 220.f;
        const float slider_height = 10.f;
        const float knob_width = 14.f;
        const float checkbox_size = 20.f;
        const float content_gap = 16.f;
        const float row_gap = 14.f;
        const float row_height = SDL_max(slider_height + 8.f, checkbox_size);

        const float volume_row_width = (float)volume_label_size.x + content_gap + slider_width + content_gap +
                                       (float)volume_value_size.x;
        const float mute_row_width = (float)mute_label_size.x + content_gap + checkbox_size;
        const float resume_row_width = (float)resume_label_size.x + content_gap + slider_width + content_gap +
                                       (float)resume_value_size.x;
        const float back_button_width = (float)back_label_size.x + 56.f;

        float content_width = SDL_max((float)title_size.x, volume_row_width);
        content_width = SDL_max(content_width, mute_row_width);
        content_width = SDL_max(content_width, resume_row_width);
        content_width = SDL_max(content_width, back_button_width);

        const float content_height = (float)title_size.y + row_gap + row_height + row_gap + row_height + row_gap +
                                     row_height + row_gap + (float)back_label_size.y + 24.f;

        ui_panel_t panel;
        ui_panel_init(&panel, k_color_menu_panel, k_color_menu_panel_border);
        vector2i_t content_size = {(int)(content_width + 0.5f), (int)(content_height + 0.5f)};
        ui_panel_layout_from_content(&panel, &screen_size_options, &content_size, 20.f, 20.f);

        if (ui_panel_render(snake->window.sdl_renderer, &panel) == false) {
            SDL_Log("Failed to render options panel: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        float cursor_y = panel.rect.y + 20.f;
        const float center_x = panel.rect.x + panel.rect.w * 0.5f;

        if (TTF_DrawRendererText(snake->hud.text_options_title, center_x - (float)title_size.x * 0.5f, cursor_y) ==
            false) {
            SDL_Log("Failed to render options title text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_y += (float)title_size.y + row_gap;

        const float row_left = center_x - content_width * 0.5f;
        const float row_center_y = cursor_y + row_height * 0.5f;
        float cursor_x = row_left;

        if (TTF_DrawRendererText(snake->hud.text_options_volume_label, cursor_x, row_center_y - volume_label_size.y * 0.5f) ==
            false) {
            SDL_Log("Failed to render volume label text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_x += (float)volume_label_size.x + content_gap;

        ui_slider_t slider;
        ui_slider_init(&slider, k_color_menu_slider_track, k_color_menu_slider_fill, k_color_menu_slider_knob,
                       k_color_menu_button_border);
        ui_slider_layout(&slider, cursor_x + slider_width * 0.5f, row_center_y, slider_width, slider_height, knob_width);
        if (ui_slider_render(snake->window.sdl_renderer, &slider, snake->config.volume) == false) {
            SDL_Log("Failed to render volume slider: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_x += slider_width + content_gap;
        if (TTF_DrawRendererText(snake->hud.text_options_volume_value, cursor_x,
                                 row_center_y - volume_value_size.y * 0.5f) == false) {
            SDL_Log("Failed to render volume value text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_y += row_height + row_gap;
        const float mute_row_center_y = cursor_y + row_height * 0.5f;
        cursor_x = row_left;

        if (TTF_DrawRendererText(snake->hud.text_options_mute_label, cursor_x, mute_row_center_y - mute_label_size.y * 0.5f) ==
            false) {
            SDL_Log("Failed to render mute label text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_x += (float)mute_label_size.x + content_gap + checkbox_size * 0.5f;
        ui_checkbox_t checkbox;
        ui_checkbox_init(&checkbox, k_color_menu_checkbox, k_color_menu_checkbox_border, k_color_menu_checkbox_check);
        ui_checkbox_layout(&checkbox, cursor_x, mute_row_center_y, checkbox_size);
        if (ui_checkbox_render(snake->window.sdl_renderer, &checkbox, snake->config.mute) == false) {
            SDL_Log("Failed to render mute checkbox: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_y += row_height + row_gap;
        const float resume_row_center_y = cursor_y + row_height * 0.5f;
        cursor_x = row_left;

        if (TTF_DrawRendererText(snake->hud.text_options_resume_label, cursor_x,
                                 resume_row_center_y - resume_label_size.y * 0.5f) == false) {
            SDL_Log("Failed to render resume label text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_x += (float)resume_label_size.x + content_gap;
        ui_slider_int_t resume_slider;
        ui_slider_int_init(&resume_slider, k_color_menu_slider_track, k_color_menu_slider_fill, k_color_menu_slider_knob,
                           k_color_menu_button_border, CONFIG_RESUME_DELAY_MIN, CONFIG_RESUME_DELAY_MAX);
        ui_slider_int_layout(&resume_slider, cursor_x + slider_width * 0.5f, resume_row_center_y, slider_width,
                             slider_height, knob_width);
        if (ui_slider_int_render(snake->window.sdl_renderer, &resume_slider, snake->config.resume_delay_seconds) ==
            false) {
            SDL_Log("Failed to render resume delay slider: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_x += slider_width + content_gap;
        if (TTF_DrawRendererText(snake->hud.text_options_resume_value, cursor_x,
                                 resume_row_center_y - resume_value_size.y * 0.5f) == false) {
            SDL_Log("Failed to render resume value text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        cursor_y += row_height + row_gap;
        ui_button_t back_button;
        ui_button_init(&back_button, k_color_menu_button, k_color_menu_button_border);
        ui_button_layout_from_label(&back_button, &back_label_size, center_x, cursor_y + back_label_size.y * 0.5f,
                                    28.f, 12.f);
        if (ui_button_render(snake->window.sdl_renderer, &back_button) == false) {
            SDL_Log("Failed to render options back button: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        float back_label_x = 0.f;
        float back_label_y = 0.f;
        ui_button_get_label_position(&back_button, &back_label_size, &back_label_x, &back_label_y);
        if (TTF_DrawRendererText(snake->hud.text_options_back_button, back_label_x, back_label_y) == false) {
            SDL_Log("Failed to render options back label: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_NONE) == false) {
            SDL_Log("Failed to reset blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }
    }

    SDL_RenderPresent(snake->window.sdl_renderer);
}
