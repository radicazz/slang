#ifndef SNAKE_STATE_H
#define SNAKE_STATE_H

#include <SDL3/SDL.h>

#include "../snake.h"

bool snake_state_reset(snake_t* snake);
void snake_state_handle_movement_key(snake_t* snake, SDL_Scancode scancode);
void snake_state_begin_resume(snake_t* snake);
void snake_state_begin_options(snake_t* snake, snake_game_state_t return_state);

#endif  // SNAKE_STATE_H
