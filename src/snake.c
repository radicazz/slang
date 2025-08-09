#include "snake.h"

#include <stdio.h>
#include <assert.h>

static ivec2_t snake_random_empty_position(snake_t* snake) {
    assert(snake != NULL);

    ivec2_t position;
    ivec2_random(&position, 1, SNAKE_WINDOW_X - 1, 1, SNAKE_WINDOW_Y - 1);

    snake_cell_t* cell = &snake->cells[position.x][position.y];
    while (cell->color != SNAKE_COLOR_BLACK) {
        ivec2_random(&position, 1, SNAKE_WINDOW_X - 1, 1, SNAKE_WINDOW_Y - 1);
        cell = &snake->cells[position.x][position.y];
    }

    return position;
}

static void handle_sdl_error(const char* message) {
    const char* error = SDL_GetError();
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", message, error);
    SDL_ClearError();
}

static void snake_reset(snake_t* snake) {
    assert(snake != NULL);

    // Avoid memory leaks when resetting the game.
    dynamic_array_destroy(&snake->food);
    dynamic_array_destroy(&snake->body);

    // Set up the cells & border.
    for (int x = 0; x < SNAKE_WINDOW_X; ++x) {
        for (int y = 0; y < SNAKE_WINDOW_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            // Set the border cells to gray, the head cell to green, and the rest to black.
            if (x == 0 || x == SNAKE_WINDOW_X - 1 || y == 0 || y == SNAKE_WINDOW_Y - 1) {
                cell->color = SNAKE_COLOR_GRAY;
            } else {
                cell->color = SNAKE_COLOR_BLACK;
            }
        }
    }

    // Generate & set the head's position.
    snake->head_position = snake_random_empty_position(snake);
    snake->cells[snake->head_position.x][snake->head_position.y].color = SNAKE_COLOR_GREEN;

    snake->previous_head_position = snake->head_position;
    snake->previous_tail_position = snake->head_position;

    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_create(&snake->food, sizeof(ivec2_t), 4);
    for (int i = 0; i < snake->food.capacity; ++i) {
        const ivec2_t food_position = snake_random_empty_position(snake);
        dynamic_array_append(&snake->food, &food_position);
        snake->cells[food_position.x][food_position.y].color = SNAKE_COLOR_RED;
    }

    dynamic_array_create(&snake->body, sizeof(ivec2_t), 8);
}

bool snake_create(snake_t* snake, const char* title) {
    assert(snake != NULL);
    assert(title != NULL);

    if (SDL_CreateWindowAndRenderer(title, SNAKE_WINDOW_WIDTH, SNAKE_WINDOW_HEIGHT, 0, &snake->window,
                                    &snake->renderer) == false) {
        handle_sdl_error("Failed to create window and renderer");
        return false;
    }

    dynamic_array_init(&snake->food);
    dynamic_array_init(&snake->body);

    snake_reset(snake);

    return snake->is_running = true;
}

void snake_destroy(snake_t* snake) {
    assert(snake != NULL);

    if (snake->renderer != NULL) {
        SDL_DestroyRenderer(snake->renderer);
        snake->renderer = NULL;
    }

    if (snake->window != NULL) {
        SDL_DestroyWindow(snake->window);
        snake->window = NULL;
    }

    ivec2_set(&snake->head_position, 0, 0);
    snake->previous_head_position = snake->head_position;
    snake->previous_tail_position = snake->head_position;

    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_destroy(&snake->food);
    dynamic_array_destroy(&snake->body);
}

static void snake_handle_key_pressed(snake_t* snake, SDL_Scancode scancode) {
    assert(snake != NULL);

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
    assert(snake != NULL);

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
    snake->cells[snake->head_position.x][snake->head_position.y].color = SNAKE_COLOR_GREEN;

    if (dynamic_array_is_empty(&snake->body) == true) {
        // Snake has no body, so clear the previous head position.
        snake->cells[snake->previous_head_position.x][snake->previous_head_position.y].color = SNAKE_COLOR_BLACK;
    } else {
        ivec2_set(&snake->previous_tail_position, 0, 0);

        // Loop over the snake's body.
        for (size_t i = 0; i < snake->body.size; ++i) {
            ivec2_t* current_body_position = (ivec2_t*)dynamic_array_get(&snake->body, i);

            if (i == 0) {
                snake->previous_tail_position = *current_body_position;
                *current_body_position = snake->previous_head_position;

                snake->cells[current_body_position->x][current_body_position->y].color = SNAKE_COLOR_GREEN;
                snake->cells[snake->previous_tail_position.x][snake->previous_tail_position.y].color =
                    SNAKE_COLOR_BLACK;
            } else {
                ivec2_t saved_position = snake->previous_tail_position;
                snake->previous_tail_position = *current_body_position;

                *current_body_position = saved_position;

                snake->cells[current_body_position->x][current_body_position->y].color = SNAKE_COLOR_GREEN;
                snake->cells[snake->previous_tail_position.x][snake->previous_tail_position.y].color =
                    SNAKE_COLOR_BLACK;
            }
        }
    }
}

bool snake_test_body_collision(snake_t* snake) {
    assert(snake != NULL);

    // TODO: Test if the snake's head collides with its body.
    return false;
}

static bool snake_test_food_collision(snake_t* snake) {
    assert(snake != NULL);

    for (size_t i = 0; i < snake->food.size; ++i) {
        ivec2_t* food_position = (ivec2_t*)dynamic_array_get(&snake->food, i);

        // Food hit.
        if (ivec2_equals(&snake->head_position, food_position) == true) {
            dynamic_array_remove(&snake->food, i);

            // Add the new food to the map.
            ivec2_t new_food_position = snake_random_empty_position(snake);
            dynamic_array_append(&snake->food, &new_food_position);
            snake->cells[new_food_position.x][new_food_position.y].color = SNAKE_COLOR_RED;

            return true;
        }
    }

    return false;
}

static void snake_update(snake_t* snake) {
    assert(snake != NULL);

    snake_move_head_and_body(snake);

    if (snake_test_body_collision(snake) == true) {
        // TODO: Handle body collision (e.g., game over)
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
    assert(snake != NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            snake->is_running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            snake_handle_key_pressed(snake, event.key.scancode);
        }
    }

    snake_update(snake);
}

void snake_render(snake_t* snake) {
    assert(snake != NULL);

    assert(snake->window != NULL);
    assert(snake->renderer != NULL);

    SDL_RenderClear(snake->renderer);

    // Loop through all the cells and render them based on their color.
    for (int x = 0; x < SNAKE_WINDOW_X; ++x) {
        for (int y = 0; y < SNAKE_WINDOW_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            // TODO: Optimize the use of SDL_SetRenderDrawColor by rendering all tiles of the same color at once.
            switch (cell->color) {
                case SNAKE_COLOR_BLACK:
                    SDL_SetRenderDrawColor(snake->renderer, 0, 0, 0, 255);
                    break;
                case SNAKE_COLOR_GRAY:
                    SDL_SetRenderDrawColor(snake->renderer, 128, 128, 128, 255);
                    break;
                case SNAKE_COLOR_GREEN:
                    SDL_SetRenderDrawColor(snake->renderer, 0, 255, 0, 255);
                    break;
                case SNAKE_COLOR_RED:
                    SDL_SetRenderDrawColor(snake->renderer, 255, 0, 0, 255);
                    break;
            }

            SDL_FRect rect;
            rect.x = cell->position.x * SNAKE_CELL_SIZE;
            rect.y = cell->position.y * SNAKE_CELL_SIZE;
            rect.w = rect.h = SNAKE_CELL_SIZE;

            SDL_RenderFillRect(snake->renderer, &rect);
        }
    }

    SDL_RenderPresent(snake->renderer);
}
