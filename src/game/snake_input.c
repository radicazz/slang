#include "snake_input.h"

#include <SDL3/SDL_log.h>

#include "snake_menu.h"
#include "snake_state.h"
#include "snake_text.h"
#include "../modules/ui.h"

static void snake_options_set_volume(snake_t* snake, float volume) {
    SDL_assert(snake != NULL);

    if (volume < 0.0f) {
        volume = 0.0f;
    } else if (volume > 1.0f) {
        volume = 1.0f;
    }

    snake->config.volume = volume;
    if (snake_apply_audio_settings(snake) == false) {
        SDL_Log("Failed to apply audio volume settings");
    }
    if (snake_text_update_options_volume(snake) == false) {
        snake->window.is_running = false;
        return;
    }
    if (snake_save_config(snake) == false) {
        SDL_Log("Failed to save config after volume change");
    }
}

static int snake_options_get_resume_delay_from_mouse(const ui_slider_int_t* slider, float mouse_x) {
    SDL_assert(slider != NULL);

    return ui_slider_int_get_value(slider, mouse_x);
}

static void snake_options_toggle_mute(snake_t* snake) {
    SDL_assert(snake != NULL);

    snake->config.mute = !snake->config.mute;
    if (snake_apply_audio_settings(snake) == false) {
        SDL_Log("Failed to apply mute setting");
    }
    if (snake_save_config(snake) == false) {
        SDL_Log("Failed to save config after mute toggle");
    }
}

static void snake_options_set_resume_delay(snake_t* snake, int seconds) {
    SDL_assert(snake != NULL);

    if (seconds < CONFIG_RESUME_DELAY_MIN) {
        seconds = CONFIG_RESUME_DELAY_MIN;
    } else if (seconds > CONFIG_RESUME_DELAY_MAX) {
        seconds = CONFIG_RESUME_DELAY_MAX;
    }

    snake->config.resume_delay_seconds = seconds;
    if (snake_text_update_options_resume_delay(snake) == false) {
        snake->window.is_running = false;
        return;
    }
    if (snake_save_config(snake) == false) {
        SDL_Log("Failed to save config after resume delay change");
    }
}

static bool snake_options_handle_mouse(snake_t* snake, float mouse_x, float mouse_y, bool pressed) {
    SDL_assert(snake != NULL);

    vector2i_t screen_size;
    if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &screen_size.x, &screen_size.y) == false) {
        SDL_Log("Failed to query render output size: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t title_size;
    if (TTF_GetTextSize(snake->text_options_title, &title_size.x, &title_size.y) == false) {
        SDL_Log("Failed to measure options title text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t volume_label_size;
    if (TTF_GetTextSize(snake->text_options_volume_label, &volume_label_size.x, &volume_label_size.y) == false) {
        SDL_Log("Failed to measure volume label text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t volume_value_size;
    if (TTF_GetTextSize(snake->text_options_volume_value, &volume_value_size.x, &volume_value_size.y) == false) {
        SDL_Log("Failed to measure volume value text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t mute_label_size;
    if (TTF_GetTextSize(snake->text_options_mute_label, &mute_label_size.x, &mute_label_size.y) == false) {
        SDL_Log("Failed to measure mute label text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t resume_label_size;
    if (TTF_GetTextSize(snake->text_options_resume_label, &resume_label_size.x, &resume_label_size.y) == false) {
        SDL_Log("Failed to measure resume delay label text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t resume_value_size;
    if (TTF_GetTextSize(snake->text_options_resume_value, &resume_value_size.x, &resume_value_size.y) == false) {
        SDL_Log("Failed to measure resume delay value text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    vector2i_t back_label_size;
    if (TTF_GetTextSize(snake->text_options_back_button, &back_label_size.x, &back_label_size.y) == false) {
        SDL_Log("Failed to measure back button text: %s", SDL_GetError());
        snake->window.is_running = false;
        return false;
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
    ui_panel_init(&panel, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    vector2i_t content_size = {(int)(content_width + 0.5f), (int)(content_height + 0.5f)};
    ui_panel_layout_from_content(&panel, &screen_size, &content_size, 20.f, 20.f);

    const float row_left = panel.rect.x + 20.f;
    const float center_x = panel.rect.x + panel.rect.w * 0.5f;
    float cursor_y = panel.rect.y + 20.f + (float)title_size.y + row_gap;
    const float row_center_y = cursor_y + row_height * 0.5f;
    float cursor_x = row_left + (float)volume_label_size.x + content_gap;

    ui_slider_t slider;
    ui_slider_init(&slider, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0},
                   (SDL_Color){0, 0, 0, 0});
    ui_slider_layout(&slider, cursor_x + slider_width * 0.5f, row_center_y, slider_width, slider_height, knob_width);

    ui_checkbox_t checkbox;
    ui_checkbox_init(&checkbox, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    cursor_y += row_height + row_gap;
    const float mute_row_center_y = cursor_y + row_height * 0.5f;
    cursor_x = row_left + (float)mute_label_size.x + content_gap + checkbox_size * 0.5f;
    ui_checkbox_layout(&checkbox, cursor_x, mute_row_center_y, checkbox_size);

    ui_slider_int_t resume_slider;
    ui_slider_int_init(&resume_slider, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0},
                       (SDL_Color){0, 0, 0, 0}, CONFIG_RESUME_DELAY_MIN, CONFIG_RESUME_DELAY_MAX);
    cursor_y += row_height + row_gap;
    const float resume_row_center_y = cursor_y + row_height * 0.5f;
    cursor_x = row_left + (float)resume_label_size.x + content_gap;
    ui_slider_int_layout(&resume_slider, cursor_x + slider_width * 0.5f, resume_row_center_y, slider_width, slider_height,
                         knob_width);

    ui_button_t back_button;
    ui_button_init(&back_button, (SDL_Color){0, 0, 0, 0}, (SDL_Color){0, 0, 0, 0});
    cursor_y += row_height + row_gap;
    ui_button_layout_from_label(&back_button, &back_label_size, center_x, cursor_y + back_label_size.y * 0.5f, 28.f,
                                12.f);

    if (pressed == true) {
        if (ui_slider_contains(&slider, mouse_x, mouse_y) == true) {
            snake->options_dragging_volume = true;
            snake->options_dragging_resume = false;
            snake_options_set_volume(snake, ui_slider_get_value(&slider, mouse_x));
            return true;
        }

        if (ui_checkbox_contains(&checkbox, mouse_x, mouse_y) == true) {
            snake_options_toggle_mute(snake);
            return true;
        }

        if (ui_slider_int_contains(&resume_slider, mouse_x, mouse_y) == true) {
            snake->options_dragging_resume = true;
            snake->options_dragging_volume = false;
            snake_options_set_resume_delay(snake, snake_options_get_resume_delay_from_mouse(&resume_slider, mouse_x));
            return true;
        }

        if (ui_button_contains(&back_button, mouse_x, mouse_y) == true) {
            snake->state = snake->options_return_state;
            if (snake->state == SNAKE_STATE_PAUSED) {
                if (snake_text_update_pause(snake) == false) {
                    snake->window.is_running = false;
                }
            }
            snake->options_dragging_volume = false;
            snake->options_dragging_resume = false;
            return true;
        }
    }

    if (snake->options_dragging_volume == true && pressed == true) {
        snake_options_set_volume(snake, ui_slider_get_value(&slider, mouse_x));
    }

    if (snake->options_dragging_resume == true && pressed == true) {
        snake_options_set_resume_delay(snake, snake_options_get_resume_delay_from_mouse(&resume_slider, mouse_x));
    }

    return true;
}

void snake_handle_events(snake_t* snake) {
    SDL_assert(snake != NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            snake->window.is_running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDL_SCANCODE_ESCAPE && event.key.repeat == 0) {
                if (snake->state == SNAKE_STATE_PLAYING) {
                    snake->state = SNAKE_STATE_PAUSED;
                    if (snake_text_update_pause(snake) == false) {
                        snake->window.is_running = false;
                    }
                } else if (snake->state == SNAKE_STATE_PAUSED) {
                    snake_state_begin_resume(snake);
                } else if (snake->state == SNAKE_STATE_OPTIONS) {
                    snake->state = snake->options_return_state;
                } else if (snake->state == SNAKE_STATE_RESUMING) {
                    snake->state = SNAKE_STATE_PAUSED;
                }
            }

            snake_state_handle_movement_key(snake, event.key.scancode);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT &&
            event.button.down == true) {
            if (snake->state == SNAKE_STATE_PAUSED) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout_with_secondary_button(snake, snake->text_pause, NULL, false,
                                                                snake->text_resume, true, snake->text_options_button,
                                                                true, &layout) == false) {
                    return;
                }

                ui_button_t button = {0};
                button.rect = layout.button_rect;
                if (ui_button_contains(&button, event.button.x, event.button.y) == true) {
                    snake_state_begin_resume(snake);
                }

                ui_button_t options_button = {0};
                options_button.rect = layout.secondary_button_rect;
                if (ui_button_contains(&options_button, event.button.x, event.button.y) == true) {
                    snake_state_begin_options(snake, SNAKE_STATE_PAUSED);
                }
            } else if (snake->state == SNAKE_STATE_START) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout_with_secondary_button(snake, snake->text_start_title,
                                                                snake->text_start_high_score, true,
                                                                snake->text_start_button, true, snake->text_options_button,
                                                                true, &layout) == false) {
                    return;
                }

                ui_button_t button = {0};
                button.rect = layout.button_rect;
                if (ui_button_contains(&button, event.button.x, event.button.y) == true) {
                    if (snake_state_reset(snake) == false) {
                        snake->window.is_running = false;
                        return;
                    }
                    snake->state = SNAKE_STATE_PLAYING;
                }

                ui_button_t options_button = {0};
                options_button.rect = layout.secondary_button_rect;
                if (ui_button_contains(&options_button, event.button.x, event.button.y) == true) {
                    snake_state_begin_options(snake, SNAKE_STATE_START);
                }
            } else if (snake->state == SNAKE_STATE_GAME_OVER) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout(snake, snake->text_game_over_title, snake->text_game_over_score, true,
                                          snake->text_restart_button, true, &layout) == false) {
                    return;
                }

                ui_button_t button = {0};
                button.rect = layout.button_rect;
                if (ui_button_contains(&button, event.button.x, event.button.y) == true) {
                    if (snake_state_reset(snake) == false) {
                        snake->window.is_running = false;
                        return;
                    }
                    snake->state = SNAKE_STATE_PLAYING;
                }
            } else if (snake->state == SNAKE_STATE_OPTIONS) {
                snake_options_handle_mouse(snake, event.button.x, event.button.y, true);
            }
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION && snake->state == SNAKE_STATE_OPTIONS) {
            snake_options_handle_mouse(snake, event.motion.x, event.motion.y, snake->options_dragging_volume);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT &&
            event.button.down == false && snake->state == SNAKE_STATE_OPTIONS) {
            snake->options_dragging_volume = false;
            snake->options_dragging_resume = false;
        }
    }
}
