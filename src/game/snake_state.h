#ifndef SNAKE_STATE_H
#define SNAKE_STATE_H

#include <SDL3/SDL.h>

#include "../snake.h"

bool snake_state_reset(snake_t* snake);
void snake_state_handle_movement_key(snake_t* snake, SDL_Scancode scancode);

#endif  // SNAKE_STATE_H
