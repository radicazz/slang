#include "snake_input.h"

#include <SDL3/SDL_log.h>

#include "snake_options_layout.h"
#include "snake_menu.h"
#include "snake_hud.h"
#include "snake_state.h"

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
    if (snake_hud_update_options_volume(&snake->hud, &snake->window, snake->config.volume) == false) {
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
    if (snake_hud_update_options_resume_delay(&snake->hud, &snake->window, snake->config.resume_delay_seconds) ==
        false) {
        snake->window.is_running = false;
        return;
    }
    if (snake_save_config(snake) == false) {
        SDL_Log("Failed to save config after resume delay change");
    }
}

static bool snake_options_handle_mouse(snake_t* snake, float mouse_x, float mouse_y, bool pressed) {
    SDL_assert(snake != NULL);

    snake_options_layout_t layout;
    if (snake_options_layout_get(snake, &layout) == false) {
        return false;
    }

    if (pressed == true) {
        if (ui_slider_contains(&layout.volume_slider, mouse_x, mouse_y) == true) {
            snake->options_dragging_volume = true;
            snake->options_dragging_resume = false;
            snake_options_set_volume(snake, ui_slider_get_value(&layout.volume_slider, mouse_x));
            return true;
        }

        if (ui_checkbox_contains(&layout.mute_checkbox, mouse_x, mouse_y) == true) {
            snake_options_toggle_mute(snake);
            return true;
        }

        if (ui_slider_int_contains(&layout.resume_slider, mouse_x, mouse_y) == true) {
            snake->options_dragging_resume = true;
            snake->options_dragging_volume = false;
            snake_options_set_resume_delay(snake,
                                           snake_options_get_resume_delay_from_mouse(&layout.resume_slider, mouse_x));
            return true;
        }

        if (ui_button_contains(&layout.back_button, mouse_x, mouse_y) == true) {
            snake->state = snake->options_return_state;
            if (snake->state == SNAKE_STATE_PAUSED) {
                if (snake_hud_update_pause(&snake->hud, snake->array_body.size) == false) {
                    snake->window.is_running = false;
                }
            }
            snake->options_dragging_volume = false;
            snake->options_dragging_resume = false;
            return true;
        }
    }

    if (snake->options_dragging_volume == true && pressed == true) {
        snake_options_set_volume(snake, ui_slider_get_value(&layout.volume_slider, mouse_x));
    }

    if (snake->options_dragging_resume == true && pressed == true) {
        snake_options_set_resume_delay(snake,
                                       snake_options_get_resume_delay_from_mouse(&layout.resume_slider, mouse_x));
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
                    snake_hud_start_menu_fade(&snake->hud);
                    if (snake_hud_update_pause(&snake->hud, snake->array_body.size) == false) {
                        snake->window.is_running = false;
                    }
                } else if (snake->state == SNAKE_STATE_PAUSED) {
                    snake_state_begin_resume(snake);
                } else if (snake->state == SNAKE_STATE_OPTIONS) {
                    snake->state = snake->options_return_state;
                    snake_hud_start_menu_fade(&snake->hud);
                } else if (snake->state == SNAKE_STATE_RESUMING) {
                    snake->state = SNAKE_STATE_PAUSED;
                    snake_hud_start_menu_fade(&snake->hud);
                }
            }

            snake_state_handle_movement_key(snake, event.key.scancode);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT &&
            event.button.down == true) {
            if (snake->state == SNAKE_STATE_PAUSED) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout_with_three_buttons(
                        snake, snake->hud.text_pause, NULL, false, snake->hud.text_resume, true,
                        snake->hud.text_options_button, true, snake->hud.text_exit_button, true, &layout) == false) {
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

                ui_button_t exit_button = {0};
                exit_button.rect = layout.tertiary_button_rect;
                if (ui_button_contains(&exit_button, event.button.x, event.button.y) == true) {
                    snake->window.is_running = false;
                }
            } else if (snake->state == SNAKE_STATE_START) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout_with_secondary_button(
                        snake, snake->hud.text_start_title, snake->hud.text_start_high_score, true,
                        snake->hud.text_start_button, true, snake->hud.text_options_button, true, &layout) == false) {
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
                if (snake_menu_get_layout(snake, snake->hud.text_game_over_title, snake->hud.text_game_over_score, true,
                                          snake->hud.text_restart_button, true, &layout) == false) {
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
            const bool dragging_slider = snake->options_dragging_volume || snake->options_dragging_resume;
            snake_options_handle_mouse(snake, event.motion.x, event.motion.y, dragging_slider);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT &&
            event.button.down == false && snake->state == SNAKE_STATE_OPTIONS) {
            snake->options_dragging_volume = false;
            snake->options_dragging_resume = false;
        }
    }
}
