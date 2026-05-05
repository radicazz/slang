#include "snake_render.h"

#include <SDL3/SDL_log.h>

#include "snake_menu.h"
#include "snake_options_layout.h"
#include "snake_util.h"
#include "../modules/ui.h"

static const SDL_Color k_color_menu_overlay = {0, 0, 0, 160};
static const SDL_Color k_color_menu_panel = {25, 25, 25, 220};
static const SDL_Color k_color_menu_panel_border = {80, 80, 80, 255};
static const SDL_Color k_color_menu_button = {45, 45, 45, 255};
static const SDL_Color k_color_menu_button_border = {100, 100, 100, 255};
static const SDL_Color k_color_menu_checkbox = {30, 30, 30, 255};
static const SDL_Color k_color_menu_checkbox_border = {100, 100, 100, 255};
static const SDL_Color k_color_menu_checkbox_check = {180, 180, 180, 255};
static const SDL_Color k_color_menu_slider_track = {40, 40, 40, 255};
static const SDL_Color k_color_menu_slider_fill = {80, 160, 100, 255};
static const SDL_Color k_color_menu_slider_knob = {180, 180, 180, 255};

void snake_render_frame(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->window.sdl_window != NULL);
    SDL_assert(snake->window.sdl_renderer != NULL);

    /* Clear the background to black (empty cell colour). */
    SDL_SetRenderDrawColor(snake->window.sdl_renderer, 0, 0, 0, 255);
    if (SDL_RenderClear(snake->window.sdl_renderer) == false) {
        SDL_Log("Failed to clear renderer: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    /* Batch rendering: all food cells share the same colour, snake cells each have
     * a unique gradient shade.  Accumulate food rects and flush with a single call;
     * render snake cells directly but avoid calling SetRenderDrawColor when the
     * colour is unchanged from the previous cell. */
    SDL_FRect food_rects[SNAKE_GRID_X * SNAKE_GRID_Y];
    int food_rect_count = 0;
    SDL_Color last_snake_color = {0, 0, 0, 0};

    for (int x = 0; x < SNAKE_GRID_X; ++x) {
        for (int y = 0; y < SNAKE_GRID_Y; ++y) {
            const snake_cell_t* const cell = &snake->cells[x][y];

            if (cell->state == SNAKE_CELL_FOOD) {
                food_rects[food_rect_count++] =
                    (SDL_FRect){(float)(x * SNAKE_CELL_SIZE), (float)(y * SNAKE_CELL_SIZE), (float)SNAKE_CELL_SIZE,
                                (float)SNAKE_CELL_SIZE};
            } else if (cell->state == SNAKE_CELL_SNAKE) {
                const SDL_Color* c = &cell->render_color;
                if (c->r != last_snake_color.r || c->g != last_snake_color.g || c->b != last_snake_color.b ||
                    c->a != last_snake_color.a) {
                    SDL_SetRenderDrawColor(snake->window.sdl_renderer, c->r, c->g, c->b, c->a);
                    last_snake_color = *c;
                }
                SDL_FRect rect = {(float)(x * SNAKE_CELL_SIZE), (float)(y * SNAKE_CELL_SIZE), (float)SNAKE_CELL_SIZE,
                                  (float)SNAKE_CELL_SIZE};
                SDL_RenderFillRect(snake->window.sdl_renderer, &rect);
            }
            /* SNAKE_CELL_EMPTY is already black from SDL_RenderClear — no draw call needed. */
        }
    }

    if (food_rect_count > 0) {
        SDL_SetRenderDrawColor(snake->window.sdl_renderer, 255, 0, 0, 255);
        SDL_RenderFillRects(snake->window.sdl_renderer, food_rects, food_rect_count);
    }

    vector2i_t screen_size;
    if (snake_get_screen_size(snake, &screen_size) == false) {
        return;
    }

    vector2i_t text_size;
    if (snake_get_text_size(snake, snake->hud.text_score, &text_size, "score") == false) {
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
        const Uint64 current_time = SDL_GetTicks();
        const Uint64 fade_duration_ms = 250;
        Uint64 elapsed = 0;

        if (current_time > snake->hud.menu_fade_start_ms) {
            elapsed = current_time - snake->hud.menu_fade_start_ms;
        }

        float fade_alpha = 1.0f;
        if (elapsed < fade_duration_ms) {
            fade_alpha = (float)elapsed / (float)fade_duration_ms;
        }
        snake->hud.menu_fade_alpha = fade_alpha;

        TTF_Text* title_text = NULL;
        TTF_Text* subtitle_text = NULL;
        bool has_subtitle = false;
        TTF_Text* button_text = NULL;
        bool has_button = false;

        if (snake->state == SNAKE_STATE_PAUSED) {
            title_text = snake->hud.text_pause;
            subtitle_text = snake->hud.text_score;
            has_subtitle = true;
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
                                                            has_button, snake->hud.text_options_button, true,
                                                            &layout) == false) {
                return;
            }
        } else {
            if (snake_menu_get_layout(snake, title_text, subtitle_text, has_subtitle, button_text, has_button,
                                      &layout) == false) {
                return;
            }
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_BLEND) == false) {
            SDL_Log("Failed to set blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        const Uint8 overlay_alpha = (Uint8)(k_color_menu_overlay.a * snake->hud.menu_fade_alpha);
        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_menu_overlay.r, k_color_menu_overlay.g,
                               k_color_menu_overlay.b, overlay_alpha);

        SDL_FRect overlay_rect = {0.f, 0.f, (float)screen_size.x, (float)screen_size.y};
        SDL_RenderFillRect(snake->window.sdl_renderer, &overlay_rect);

        ui_panel_t panel;
        ui_panel_init(&panel, k_color_menu_panel, k_color_menu_panel_border);
        panel.rect = layout.panel_rect;
        panel.fill_color.a = (Uint8)(k_color_menu_panel.a * snake->hud.menu_fade_alpha);
        panel.border_color.a = (Uint8)(k_color_menu_panel_border.a * snake->hud.menu_fade_alpha);
        if (ui_panel_render(snake->window.sdl_renderer, &panel) == false) {
            SDL_Log("Failed to render menu panel: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        ui_button_t button;
        if (layout.has_button == true) {
            ui_button_init(&button, k_color_menu_button, k_color_menu_button_border);
            button.rect = layout.button_rect;
            button.fill_color.a = (Uint8)(k_color_menu_button.a * snake->hud.menu_fade_alpha);
            button.border_color.a = (Uint8)(k_color_menu_button_border.a * snake->hud.menu_fade_alpha);
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
            float countdown_x = (float)(screen_size.x - snake->hud.text_resume_countdown_size.x) * 0.5f;
            float countdown_y = layout.title_pos.y + 60.f;
            SDL_FRect dst = {countdown_x, countdown_y, (float)snake->hud.text_resume_countdown_size.x,
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
            if (snake_get_text_size(snake, button_text, &button_text_size, "menu button") == false) {
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
            if (snake_get_text_size(snake, snake->hud.text_options_button, &options_text_size, "options button") ==
                false) {
                return;
            }

            ui_button_t options_button;
            ui_button_init(&options_button, k_color_menu_button, k_color_menu_button_border);
            options_button.rect = layout.secondary_button_rect;
            options_button.fill_color.a = (Uint8)(k_color_menu_button.a * snake->hud.menu_fade_alpha);
            options_button.border_color.a = (Uint8)(k_color_menu_button_border.a * snake->hud.menu_fade_alpha);
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
            if (snake_get_text_size(snake, snake->hud.text_exit_button, &exit_text_size, "exit button") == false) {
                return;
            }

            ui_button_t exit_button;
            ui_button_init(&exit_button, k_color_menu_button, k_color_menu_button_border);
            exit_button.rect = layout.tertiary_button_rect;
            exit_button.fill_color.a = (Uint8)(k_color_menu_button.a * snake->hud.menu_fade_alpha);
            exit_button.border_color.a = (Uint8)(k_color_menu_button_border.a * snake->hud.menu_fade_alpha);
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
        const Uint64 current_time = SDL_GetTicks();
        const Uint64 fade_duration_ms = 250;
        Uint64 elapsed = 0;

        if (current_time > snake->hud.menu_fade_start_ms) {
            elapsed = current_time - snake->hud.menu_fade_start_ms;
        }

        float fade_alpha = 1.0f;
        if (elapsed < fade_duration_ms) {
            fade_alpha = (float)elapsed / (float)fade_duration_ms;
        }
        snake->hud.menu_fade_alpha = fade_alpha;

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_BLEND) == false) {
            SDL_Log("Failed to set blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        const Uint8 overlay_alpha_options = (Uint8)(k_color_menu_overlay.a * snake->hud.menu_fade_alpha);
        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_menu_overlay.r, k_color_menu_overlay.g,
                               k_color_menu_overlay.b, overlay_alpha_options);
        SDL_FRect overlay_rect = {0.f, 0.f, (float)screen_size.x, (float)screen_size.y};
        SDL_RenderFillRect(snake->window.sdl_renderer, &overlay_rect);

        snake_options_layout_t options_layout;
        if (snake_options_layout_get(snake, &options_layout) == false) {
            return;
        }

        options_layout.panel.fill_color = k_color_menu_panel;
        options_layout.panel.border_color = k_color_menu_panel_border;
        options_layout.panel.fill_color.a = (Uint8)(k_color_menu_panel.a * snake->hud.menu_fade_alpha);
        options_layout.panel.border_color.a = (Uint8)(k_color_menu_panel_border.a * snake->hud.menu_fade_alpha);

        if (ui_panel_render(snake->window.sdl_renderer, &options_layout.panel) == false) {
            SDL_Log("Failed to render options panel: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_title, options_layout.title_pos.x,
                                 options_layout.title_pos.y) == false) {
            SDL_Log("Failed to render options title text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_volume_label, options_layout.volume_label_pos.x,
                                 options_layout.volume_label_pos.y) == false) {
            SDL_Log("Failed to render volume label text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        options_layout.volume_slider.track_color = k_color_menu_slider_track;
        options_layout.volume_slider.fill_color = k_color_menu_slider_fill;
        options_layout.volume_slider.knob_color = k_color_menu_slider_knob;
        options_layout.volume_slider.border_color = k_color_menu_button_border;
        options_layout.volume_slider.track_color.a = (Uint8)(k_color_menu_slider_track.a * snake->hud.menu_fade_alpha);
        options_layout.volume_slider.fill_color.a = (Uint8)(k_color_menu_slider_fill.a * snake->hud.menu_fade_alpha);
        options_layout.volume_slider.knob_color.a = (Uint8)(k_color_menu_slider_knob.a * snake->hud.menu_fade_alpha);
        options_layout.volume_slider.border_color.a =
            (Uint8)(k_color_menu_button_border.a * snake->hud.menu_fade_alpha);
        if (ui_slider_render(snake->window.sdl_renderer, &options_layout.volume_slider, snake->config.volume) ==
            false) {
            SDL_Log("Failed to render volume slider: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_volume_value, options_layout.volume_value_pos.x,
                                 options_layout.volume_value_pos.y) == false) {
            SDL_Log("Failed to render volume value text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_mute_label, options_layout.mute_label_pos.x,
                                 options_layout.mute_label_pos.y) == false) {
            SDL_Log("Failed to render mute label text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        options_layout.mute_checkbox.fill_color = k_color_menu_checkbox;
        options_layout.mute_checkbox.border_color = k_color_menu_checkbox_border;
        options_layout.mute_checkbox.check_color = k_color_menu_checkbox_check;
        options_layout.mute_checkbox.fill_color.a = (Uint8)(k_color_menu_checkbox.a * snake->hud.menu_fade_alpha);
        options_layout.mute_checkbox.border_color.a =
            (Uint8)(k_color_menu_checkbox_border.a * snake->hud.menu_fade_alpha);
        options_layout.mute_checkbox.check_color.a =
            (Uint8)(k_color_menu_checkbox_check.a * snake->hud.menu_fade_alpha);
        if (ui_checkbox_render(snake->window.sdl_renderer, &options_layout.mute_checkbox, snake->config.mute) ==
            false) {
            SDL_Log("Failed to render mute checkbox: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_resume_label, options_layout.resume_label_pos.x,
                                 options_layout.resume_label_pos.y) == false) {
            SDL_Log("Failed to render resume label text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        options_layout.resume_slider.slider.track_color = k_color_menu_slider_track;
        options_layout.resume_slider.slider.fill_color = k_color_menu_slider_fill;
        options_layout.resume_slider.slider.knob_color = k_color_menu_slider_knob;
        options_layout.resume_slider.slider.border_color = k_color_menu_button_border;
        options_layout.resume_slider.slider.track_color.a =
            (Uint8)(k_color_menu_slider_track.a * snake->hud.menu_fade_alpha);
        options_layout.resume_slider.slider.fill_color.a =
            (Uint8)(k_color_menu_slider_fill.a * snake->hud.menu_fade_alpha);
        options_layout.resume_slider.slider.knob_color.a =
            (Uint8)(k_color_menu_slider_knob.a * snake->hud.menu_fade_alpha);
        options_layout.resume_slider.slider.border_color.a =
            (Uint8)(k_color_menu_button_border.a * snake->hud.menu_fade_alpha);
        if (ui_slider_int_render(snake->window.sdl_renderer, &options_layout.resume_slider,
                                 snake->config.resume_delay_seconds) == false) {
            SDL_Log("Failed to render resume delay slider: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_resume_value, options_layout.resume_value_pos.x,
                                 options_layout.resume_value_pos.y) == false) {
            SDL_Log("Failed to render resume value text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        options_layout.back_button.fill_color = k_color_menu_button;
        options_layout.back_button.border_color = k_color_menu_button_border;
        options_layout.back_button.fill_color.a = (Uint8)(k_color_menu_button.a * snake->hud.menu_fade_alpha);
        options_layout.back_button.border_color.a = (Uint8)(k_color_menu_button_border.a * snake->hud.menu_fade_alpha);
        if (ui_button_render(snake->window.sdl_renderer, &options_layout.back_button) == false) {
            SDL_Log("Failed to render options back button: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(snake->hud.text_options_back_button, options_layout.back_label_pos.x,
                                 options_layout.back_label_pos.y) == false) {
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
