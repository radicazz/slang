#include "snake.h"

#include <stdio.h>
#include <assert.h>

// NOTE: This function will eventually break the game at the late stages
// when there are no more empty positions available as it will get stuck
// in the while loop.
//
// TODO: add a check for the amount of available positions on the board.
static ivec2_t snake_random_empty_position(snake_t* snake) {
    SDL_assert(snake != NULL);

    ivec2_t position;
    ivec2_random(&position, SNAKE_WINDOW_X - 1, SNAKE_WINDOW_Y - 1);

    snake_cell_t* cell = &snake->cells[position.x][position.y];
    while (cell->color != SNAKE_COLOR_BLACK) {
        ivec2_random(&position, SNAKE_WINDOW_X - 1, SNAKE_WINDOW_Y - 1);
        cell = &snake->cells[position.x][position.y];
    }

    return position;
}

static void snake_cell_set_color(snake_t* snake, const ivec2_t* position, snake_colors_t color) {
    SDL_assert(snake != NULL);
    SDL_assert(position != NULL);

    snake->cells[position->x][position->y].color = color;
}

static void snake_reset(snake_t* snake) {
    SDL_assert(snake != NULL);

    dynamic_array_destroy(&snake->food);
    dynamic_array_destroy(&snake->body);

    for (int x = 0; x < SNAKE_WINDOW_X; ++x) {
        for (int y = 0; y < SNAKE_WINDOW_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            // Set the border cells to gray and the rest to black.
            if (x == 0 || x == SNAKE_WINDOW_X - 1 || y == 0 || y == SNAKE_WINDOW_Y - 1) {
                cell->color = SNAKE_COLOR_GRAY;
            } else {
                cell->color = SNAKE_COLOR_BLACK;
            }
        }
    }

    snake->head_position = snake_random_empty_position(snake);
    snake_cell_set_color(snake, &snake->head_position, SNAKE_COLOR_GREEN);
    snake->previous_head_position = snake->head_position;
    snake->previous_tail_position = snake->head_position;
    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_create(&snake->food, sizeof(ivec2_t), 8);
    for (int i = 0; i < snake->food.capacity; ++i) {
        const ivec2_t food_position = snake_random_empty_position(snake);
        dynamic_array_append(&snake->food, &food_position);
        snake_cell_set_color(snake, &food_position, SNAKE_COLOR_RED);
    }

    dynamic_array_create(&snake->body, sizeof(ivec2_t), 8);
}

bool snake_create(snake_t* snake, const char* title) {
    SDL_assert(snake != NULL);
    SDL_assert(title != NULL);

    if (app_create(&snake->app, title, SNAKE_WINDOW_WIDTH, SNAKE_WINDOW_HEIGHT) == false) {
        return false;
    }

    dynamic_array_init(&snake->food);
    dynamic_array_init(&snake->body);

    snake_reset(snake);

    return snake->app.is_running;
}

void snake_destroy(snake_t* snake) {
    SDL_assert(snake != NULL);

    app_destroy(&snake->app);

    ivec2_set(&snake->head_position, 0, 0);
    snake->previous_head_position = snake->head_position;
    snake->previous_tail_position = snake->head_position;

    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_destroy(&snake->food);
    dynamic_array_destroy(&snake->body);
}

static void snake_handle_key_pressed(snake_t* snake, SDL_Scancode scancode) {
    SDL_assert(snake != NULL);

    switch (scancode) {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            if (snake->current_direction != SNAKE_DIRECTION_DOWN) {
                snake->current_direction = SNAKE_DIRECTION_UP;
            }
            break;

        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            if (snake->current_direction != SNAKE_DIRECTION_UP) {
                snake->current_direction = SNAKE_DIRECTION_DOWN;
            }
            break;

        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            if (snake->current_direction != SNAKE_DIRECTION_RIGHT) {
                snake->current_direction = SNAKE_DIRECTION_LEFT;
            }
            break;

        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            if (snake->current_direction != SNAKE_DIRECTION_LEFT) {
                snake->current_direction = SNAKE_DIRECTION_RIGHT;
            }
            break;
    }
}

static void snake_move_head_and_body(snake_t* snake) {
    SDL_assert(snake != NULL);

    // Save the previous head position.
    snake->previous_head_position = snake->head_position;

    // Temporary head position to test collisions with border.
    ivec2_t new_head_position = snake->head_position;

    switch (snake->current_direction) {
        case SNAKE_DIRECTION_UP:
            new_head_position.y -= 1;
            break;
        case SNAKE_DIRECTION_DOWN:
            new_head_position.y += 1;
            break;
        case SNAKE_DIRECTION_LEFT:
            new_head_position.x -= 1;
            break;
        case SNAKE_DIRECTION_RIGHT:
            new_head_position.x += 1;
            break;
    }

    // Wrap around the screen edges.
    if (new_head_position.x == 0) {
        new_head_position.x = SNAKE_WINDOW_X - 2;
    }

    if (new_head_position.y == 0) {
        new_head_position.y = SNAKE_WINDOW_Y - 2;
    }

    if (new_head_position.x == SNAKE_WINDOW_X - 1) {
        new_head_position.x = 1;
    }

    if (new_head_position.y == SNAKE_WINDOW_Y - 1) {
        new_head_position.y = 1;
    }

    // Move the snake's head.
    snake->head_position = new_head_position;
    snake_cell_set_color(snake, &snake->head_position, SNAKE_COLOR_GREEN);

    if (dynamic_array_is_empty(&snake->body) == true) {
        // Snake has no body, so clear the previous head position.
        snake_cell_set_color(snake, &snake->previous_head_position, SNAKE_COLOR_BLACK);
    } else {
        ivec2_set(&snake->previous_tail_position, 0, 0);

        // Loop over the snake's body.
        for (size_t i = 0; i < snake->body.size; ++i) {
            ivec2_t* const current_body_position = (ivec2_t* const)dynamic_array_get(&snake->body, i);

            if (i == 0) {
                snake->previous_tail_position = *current_body_position;
                *current_body_position = snake->previous_head_position;

                snake_cell_set_color(snake, current_body_position, SNAKE_COLOR_GREEN);
                snake_cell_set_color(snake, &snake->previous_tail_position, SNAKE_COLOR_BLACK);
            } else {
                ivec2_t saved_position = snake->previous_tail_position;
                snake->previous_tail_position = *current_body_position;

                *current_body_position = saved_position;

                snake_cell_set_color(snake, current_body_position, SNAKE_COLOR_GREEN);
                snake_cell_set_color(snake, &snake->previous_tail_position, SNAKE_COLOR_BLACK);
            }
        }
    }
}

bool snake_test_body_collision(snake_t* snake) {
    SDL_assert(snake != NULL);

    for (size_t i = 0; i < snake->body.size; ++i) {
        const ivec2_t* const body_segment = (const ivec2_t* const)dynamic_array_get(&snake->body, i);
        if (ivec2_equals(&snake->head_position, body_segment) == true) {
            return true;
        }
    }

    return false;
}

static bool snake_test_food_collision(snake_t* snake) {
    SDL_assert(snake != NULL);

    for (size_t i = 0; i < snake->food.size; ++i) {
        const ivec2_t* const food_position = (const ivec2_t* const)dynamic_array_get(&snake->food, i);

        // Food hit.
        if (ivec2_equals(&snake->head_position, food_position) == true) {
            dynamic_array_remove(&snake->food, i);

            // Add the new food to the map.
            const ivec2_t new_food_position = snake_random_empty_position(snake);
            dynamic_array_append(&snake->food, &new_food_position);
            snake_cell_set_color(snake, &new_food_position, SNAKE_COLOR_RED);

            return true;
        }
    }

    return false;
}

void snake_update(snake_t* snake) {
    SDL_assert(snake != NULL);

    snake_move_head_and_body(snake);

    if (snake_test_body_collision(snake) == true) {
        // Restart the game if the snake collides with its own body.
        snake_reset(snake);
        return;
    }

    // Grow the snake if it hits food.
    if (snake_test_food_collision(snake) == true) {
        ivec2_t new_segment_position;
        if (dynamic_array_is_empty(&snake->body) == true) {
            new_segment_position = snake->previous_head_position;
        } else {
            new_segment_position = snake->previous_tail_position;
        }

        dynamic_array_append(&snake->body, &new_segment_position);
    }
}

void snake_handle_events(snake_t* snake) {
    SDL_assert(snake != NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            snake->app.is_running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            snake_handle_key_pressed(snake, event.key.scancode);
        }
    }
}

void snake_render(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->app.window != NULL);
    SDL_assert(snake->app.renderer != NULL);

    SDL_RenderClear(snake->app.renderer);

    // Loop through all the cells and render them based on their color.
    for (int x = 0; x < SNAKE_WINDOW_X; ++x) {
        for (int y = 0; y < SNAKE_WINDOW_Y; ++y) {
            const snake_cell_t* const cell = &snake->cells[x][y];

            // TODO: Optimize the use of SDL_SetRenderDrawColor by rendering all tiles of the same color at once.
            switch (cell->color) {
                case SNAKE_COLOR_BLACK:
                    SDL_SetRenderDrawColor(snake->app.renderer, 0, 0, 0, 255);
                    break;
                case SNAKE_COLOR_GRAY:
                    SDL_SetRenderDrawColor(snake->app.renderer, 128, 128, 128, 255);
                    break;
                case SNAKE_COLOR_GREEN:
                    SDL_SetRenderDrawColor(snake->app.renderer, 0, 255, 0, 255);
                    break;
                case SNAKE_COLOR_RED:
                    SDL_SetRenderDrawColor(snake->app.renderer, 255, 0, 0, 255);
                    break;
            }

            SDL_FRect rect;
            rect.x = cell->position.x * SNAKE_CELL_SIZE;
            rect.y = cell->position.y * SNAKE_CELL_SIZE;
            rect.w = rect.h = SNAKE_CELL_SIZE;

            SDL_RenderFillRect(snake->app.renderer, &rect);
        }
    }

    SDL_RenderPresent(snake->app.renderer);
}
