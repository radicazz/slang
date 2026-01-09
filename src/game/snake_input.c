#include "snake_input.h"

#include <SDL3/SDL_log.h>

#include "snake_menu.h"
#include "snake_state.h"
#include "snake_text.h"
#include "../modules/ui.h"

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
                    snake->state = SNAKE_STATE_PLAYING;
                }
            }

            snake_state_handle_movement_key(snake, event.key.scancode);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT &&
            event.button.down == true) {
            if (snake->state == SNAKE_STATE_PAUSED) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout(snake, snake->text_pause, NULL, false, snake->text_resume, &layout) ==
                    false) {
                    return;
                }

                ui_button_t button = {0};
                button.rect = layout.button_rect;
                if (ui_button_contains(&button, event.button.x, event.button.y) == true) {
                    snake->state = SNAKE_STATE_PLAYING;
                }
            } else if (snake->state == SNAKE_STATE_START) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout(snake, snake->text_start_title, NULL, false, snake->text_start_button,
                                          &layout) == false) {
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
            } else if (snake->state == SNAKE_STATE_GAME_OVER) {
                snake_menu_layout_t layout;
                if (snake_menu_get_layout(snake, snake->text_game_over_title, snake->text_game_over_score, true,
                                          snake->text_restart_button, &layout) == false) {
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
            }
        }
    }
}
