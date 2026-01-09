#include "snake_state.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

#include "snake_text.h"

static const SDL_Color k_color_empty = {0, 0, 0, 255};
static const SDL_Color k_color_food = {255, 0, 0, 255};
static const SDL_Color k_color_snake_head = {0, 255, 0, 255};

static bool get_random_empty_position(snake_t* snake, vector2i_t* out_position) {
    SDL_assert(snake != NULL);
    SDL_assert(out_position != NULL);

    const int interior_width = SNAKE_GRID_X - 2;
    const int interior_height = SNAKE_GRID_Y - 2;
    const int max_attempts = interior_width * interior_height * 2;

    vector2i_t position;
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        vector2i_random(&position, interior_width, interior_height);
        snake_cell_t* cell = &snake->cells[position.x][position.y];
        if (cell->state == SNAKE_CELL_EMPTY) {
            *out_position = position;
            return true;
        }
    }

    for (int x = 1; x < SNAKE_GRID_X - 1; ++x) {
        for (int y = 1; y < SNAKE_GRID_Y - 1; ++y) {
            if (snake->cells[x][y].state == SNAKE_CELL_EMPTY) {
                vector2i_set(out_position, x, y);
                return true;
            }
        }
    }

    SDL_Log("Failed to find empty position on grid (grid full or no space available)");
    return false;
}

static void cell_set_state_and_color(snake_t* snake, const vector2i_t* position, snake_cell_state_t state,
                                     const SDL_Color* color) {
    SDL_assert(snake != NULL);
    SDL_assert(position != NULL);

    snake_cell_t* const cell = &snake->cells[position->x][position->y];
    cell->state = state;
    if (color != NULL) {
        cell->render_color = *color;
    }
}

static void move_head_and_body(snake_t* snake) {
    SDL_assert(snake != NULL);

    // Save the previous head position.
    snake->previous_position_head = snake->position_head;

    // Temporary head position to test collisions with border.
    vector2i_t new_head_position = snake->position_head;

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
        new_head_position.x = SNAKE_GRID_X - 2;
    }

    if (new_head_position.y == 0) {
        new_head_position.y = SNAKE_GRID_Y - 2;
    }

    if (new_head_position.x == SNAKE_GRID_X - 1) {
        new_head_position.x = 1;
    }

    if (new_head_position.y == SNAKE_GRID_Y - 1) {
        new_head_position.y = 1;
    }

    // Move the snake's head.
    snake->position_head = new_head_position;
    cell_set_state_and_color(snake, &snake->position_head, SNAKE_CELL_SNAKE, &k_color_snake_head);

    if (dynamic_array_is_empty(&snake->array_body) == true) {
        // Snake has no array_body, so clear the previous head position.
        cell_set_state_and_color(snake, &snake->previous_position_head, SNAKE_CELL_EMPTY, &k_color_empty);
    } else {
        vector2i_set(&snake->previous_position_tail, 0, 0);

        // Loop over the snake's array_body.
        for (size_t i = 0; i < snake->array_body.size; ++i) {
            vector2i_t* const current_body_position = (vector2i_t* const)dynamic_array_get(&snake->array_body, i);

            if (i == 0) {
                snake->previous_position_tail = *current_body_position;
                *current_body_position = snake->previous_position_head;
            } else {
                vector2i_t saved_position = snake->previous_position_tail;
                snake->previous_position_tail = *current_body_position;

                *current_body_position = saved_position;
            }

            cell_set_state_and_color(snake, current_body_position, SNAKE_CELL_SNAKE, NULL);
            cell_set_state_and_color(snake, &snake->previous_position_tail, SNAKE_CELL_EMPTY, &k_color_empty);
        }
    }
}

static void update_snake_gradient(snake_t* snake) {
    SDL_assert(snake != NULL);

    const Uint8 head_green = 255;
    const Uint8 tail_green = 120;
    const float knee = 0.3f;
    const float knee_weight = 0.7f;

    snake_cell_t* const head_cell = &snake->cells[snake->position_head.x][snake->position_head.y];
    head_cell->state = SNAKE_CELL_SNAKE;
    head_cell->render_color.r = 0;
    head_cell->render_color.g = head_green;
    head_cell->render_color.b = 0;
    head_cell->render_color.a = 255;

    if (snake->array_body.size == 0) {
        return;
    }

    const float start = (float)head_green;
    const float end = (float)tail_green;
    const float length = (float)snake->array_body.size;

    for (size_t i = 0; i < snake->array_body.size; ++i) {
        vector2i_t* const body_position = (vector2i_t* const)dynamic_array_get(&snake->array_body, i);
        snake_cell_t* const cell = &snake->cells[body_position->x][body_position->y];

        const float t = (float)(i + 1) / length;
        float eased_t;
        if (t <= knee) {
            eased_t = (t / knee) * knee_weight;
        } else {
            eased_t = knee_weight + ((t - knee) / (1.0f - knee)) * (1.0f - knee_weight);
        }

        float green_value = start + (end - start) * eased_t;
        if (green_value < 0.f) {
            green_value = 0.f;
        } else if (green_value > 255.f) {
            green_value = 255.f;
        }

        cell->state = SNAKE_CELL_SNAKE;
        cell->render_color.r = 0;
        cell->render_color.g = (Uint8)(green_value + 0.5f);
        cell->render_color.b = 0;
        cell->render_color.a = 255;
    }
}

static bool test_body_collision(snake_t* snake) {
    SDL_assert(snake != NULL);

    for (size_t i = 0; i < snake->array_body.size; ++i) {
        const vector2i_t* const body_segment = (const vector2i_t* const)dynamic_array_get(&snake->array_body, i);
        if (vector2i_equals(&snake->position_head, body_segment) == true) {
            return true;
        }
    }

    return false;
}

static bool test_food_collision(snake_t* snake) {
    SDL_assert(snake != NULL);

    for (size_t i = 0; i < snake->array_food.size; ++i) {
        const vector2i_t* const food_position = (const vector2i_t* const)dynamic_array_get(&snake->array_food, i);

        // Food hit.
        if (vector2i_equals(&snake->position_head, food_position) == true) {
            dynamic_array_remove(&snake->array_food, i);

            vector2i_t new_food_position;
            if (get_random_empty_position(snake, &new_food_position) == true) {
                dynamic_array_append(&snake->array_food, &new_food_position);
                cell_set_state_and_color(snake, &new_food_position, SNAKE_CELL_FOOD, &k_color_food);
            }

            return true;
        }
    }

    return false;
}

static int get_resume_seconds_remaining(Uint64 now_ms, Uint64 end_ms) {
    if (now_ms >= end_ms) {
        return 0;
    }

    const Uint64 remaining_ms = end_ms - now_ms;
    return (int)((remaining_ms + 999u) / 1000u);
}

bool snake_state_reset(snake_t* snake) {
    SDL_assert(snake != NULL);

    SDL_Log("Resetting game state");

    dynamic_array_destroy(&snake->array_food);
    dynamic_array_destroy(&snake->array_body);

    for (int x = 0; x < SNAKE_GRID_X; ++x) {
        for (int y = 0; y < SNAKE_GRID_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            cell_set_state_and_color(snake, &cell->position, SNAKE_CELL_EMPTY, &k_color_empty);
        }
    }

    vector2i_t head_position;
    if (get_random_empty_position(snake, &head_position) == false) {
        SDL_Log("Failed to find starting position for snake head");
        return false;
    }

    snake->position_head = head_position;
    cell_set_state_and_color(snake, &snake->position_head, SNAKE_CELL_SNAKE, &k_color_snake_head);
    snake->previous_position_head = snake->position_head;
    snake->previous_position_tail = snake->position_head;
    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_create(&snake->array_food, sizeof(vector2i_t), 8);
    for (int i = 0; i < snake->array_food.capacity; ++i) {
        vector2i_t food_position;
        if (get_random_empty_position(snake, &food_position) == false) {
            SDL_Log("Warning: Could only spawn %d food items", i);
            break;
        }

        dynamic_array_append(&snake->array_food, &food_position);
        cell_set_state_and_color(snake, &food_position, SNAKE_CELL_FOOD, &k_color_food);
    }

    dynamic_array_create(&snake->array_body, sizeof(vector2i_t), 8);

    if (snake_text_update_score(snake) == false) {
        dynamic_array_destroy(&snake->array_food);
        dynamic_array_destroy(&snake->array_body);
        return false;
    }

    SDL_Log("Game reset complete (food items: %zu)", snake->array_food.size);
    return true;
}

void snake_state_handle_movement_key(snake_t* snake, SDL_Scancode scancode) {
    SDL_assert(snake != NULL);

    if (snake->state != SNAKE_STATE_PLAYING) {
        return;
    }

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
        default:
            break;
    }
}

void snake_state_begin_resume(snake_t* snake) {
    SDL_assert(snake != NULL);

    snake->state = SNAKE_STATE_RESUMING;
    snake->resume_countdown_end_ms = SDL_GetTicks() + 3000u;
    snake->resume_countdown_value = -1;

    const int seconds = get_resume_seconds_remaining(SDL_GetTicks(), snake->resume_countdown_end_ms);
    if (snake_text_update_resume_countdown(snake, seconds) == false) {
        snake->window.is_running = false;
    } else {
        snake->resume_countdown_value = seconds;
    }
}

void snake_update_fixed(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (snake->state == SNAKE_STATE_RESUMING) {
        const Uint64 now_ms = SDL_GetTicks();
        const int seconds = get_resume_seconds_remaining(now_ms, snake->resume_countdown_end_ms);
        if (seconds != snake->resume_countdown_value) {
            if (snake_text_update_resume_countdown(snake, seconds) == false) {
                snake->window.is_running = false;
                return;
            }
            snake->resume_countdown_value = seconds;
        }

        if (now_ms >= snake->resume_countdown_end_ms) {
            snake->state = SNAKE_STATE_PLAYING;
        }
        return;
    }

    if (snake->state != SNAKE_STATE_PLAYING) {
        return;
    }

    move_head_and_body(snake);

    if (test_body_collision(snake) == true) {
        SDL_Log("Collision detected! Score: %zu", snake->array_body.size);
        snake->state = SNAKE_STATE_GAME_OVER;
        if (snake_text_update_game_over(snake) == false) {
            snake->window.is_running = false;
        }
        return;
    }

    // Grow the snake if it hits array_food.
    if (test_food_collision(snake) == true) {
        audio_manager_play_sound(&snake->audio, SOUND_EAT_FOOD);

        vector2i_t new_segment_position;
        if (dynamic_array_is_empty(&snake->array_body) == true) {
            new_segment_position = snake->previous_position_head;
        } else {
            new_segment_position = snake->previous_position_tail;
        }

        dynamic_array_append(&snake->array_body, &new_segment_position);

        if (snake_text_update_score(snake) == false) {
            snake->window.is_running = false;
        }
    }

    update_snake_gradient(snake);
}
