#include "snake.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL3/SDL_log.h>

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
        if (cell->color == SNAKE_COLOR_BLACK) {
            *out_position = position;
            return true;
        }
    }

    for (int x = 1; x < SNAKE_GRID_X - 1; ++x) {
        for (int y = 1; y < SNAKE_GRID_Y - 1; ++y) {
            if (snake->cells[x][y].color == SNAKE_COLOR_BLACK) {
                vector2i_set(out_position, x, y);
                return true;
            }
        }
    }

    SDL_Log("Failed to find empty position on grid (grid full or no space available)");
    return false;
}

static SDL_Color color_to_render_color(snake_colors_t color) {
    SDL_Color render_color = {0, 0, 0, 255};

    switch (color) {
        case SNAKE_COLOR_BLACK:
            render_color.r = 0;
            render_color.g = 0;
            render_color.b = 0;
            break;
        case SNAKE_COLOR_GRAY:
            render_color.r = 50;
            render_color.g = 50;
            render_color.b = 50;
            break;
        case SNAKE_COLOR_GREEN:
            render_color.r = 0;
            render_color.g = 255;
            render_color.b = 0;
            break;
        case SNAKE_COLOR_DARK_GREEN:
            render_color.r = 0;
            render_color.g = 180;
            render_color.b = 0;
            break;
        case SNAKE_COLOR_RED:
            render_color.r = 255;
            render_color.g = 0;
            render_color.b = 0;
            break;
    }

    return render_color;
}

static void cell_set_color(snake_t* snake, const vector2i_t* position, snake_colors_t color) {
    SDL_assert(snake != NULL);
    SDL_assert(position != NULL);

    snake_cell_t* const cell = &snake->cells[position->x][position->y];
    cell->color = color;
    cell->render_color = color_to_render_color(color);
}

static bool update_score_text(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_score != NULL);

    const int written =
        snprintf(snake->text_score_buffer, sizeof(snake->text_score_buffer), "Score: %zu", snake->array_body.size);
    if (written < 0 || (size_t)written >= sizeof(snake->text_score_buffer)) {
        SDL_Log("Failed to format score text.");
        return false;
    }

    if (TTF_SetTextString(snake->text_score, snake->text_score_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update score text: %s", SDL_GetError());
        return false;
    }

    return true;
}

static bool reset(snake_t* snake) {
    SDL_assert(snake != NULL);

    SDL_Log("Resetting game state");

    snake->is_paused = false;

    dynamic_array_destroy(&snake->array_food);
    dynamic_array_destroy(&snake->array_body);

    for (int x = 0; x < SNAKE_GRID_X; ++x) {
        for (int y = 0; y < SNAKE_GRID_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            // Set the border cells to gray and the rest to black.
            if (x == 0 || x == SNAKE_GRID_X - 1 || y == 0 || y == SNAKE_GRID_Y - 1) {
                cell_set_color(snake, &cell->position, SNAKE_COLOR_GRAY);
            } else {
                cell_set_color(snake, &cell->position, SNAKE_COLOR_BLACK);
            }
        }
    }

    vector2i_t head_position;
    if (get_random_empty_position(snake, &head_position) == false) {
        SDL_Log("Failed to find starting position for snake head");
        return false;
    }

    snake->position_head = head_position;
    cell_set_color(snake, &snake->position_head, SNAKE_COLOR_GREEN);
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
        cell_set_color(snake, &food_position, SNAKE_COLOR_RED);
    }

    dynamic_array_create(&snake->array_body, sizeof(vector2i_t), 8);

    if (update_score_text(snake) == false) {
        dynamic_array_destroy(&snake->array_food);
        dynamic_array_destroy(&snake->array_body);
        return false;
    }

    SDL_Log("Game reset complete (food items: %zu)", snake->array_food.size);
    return true;
}

bool snake_create(snake_t* snake, const char* title) {
    SDL_assert(snake != NULL);
    SDL_assert(title != NULL);

    SDL_Log("Initializing snake game");

    memset(snake, 0, sizeof(*snake));

    if (window_create(&snake->window, title, WINDOW_WIDTH, WINDOW_HEIGHT) == false) {
        SDL_Log("Failed to create game window");
        return false;
    }

    if (audio_manager_create(&snake->audio) == false) {
        SDL_Log("Warning: Failed to initialize audio, continuing without sound");
    } else {
        if (audio_manager_load_sound(&snake->audio, SOUND_EAT_FOOD, "assets/sounds/bubble-pop.wav") == false) {
            SDL_Log("Warning: Failed to load eating sound effect");
        }
    }

    Uint64 seed = SDL_GetTicksNS();
    seed ^= SDL_GetPerformanceCounter();
    seed ^= (Uint64)(uintptr_t)snake;
    if (seed == 0) {
        seed = SDL_GetPerformanceCounter() | 1u;
    }
    SDL_srand(seed);
    SDL_Log("RNG initialized with seed: %llu", (unsigned long long)seed);

    dynamic_array_init(&snake->array_food);
    dynamic_array_init(&snake->array_body);

    const int written =
        snprintf(snake->text_score_buffer, sizeof(snake->text_score_buffer), "Score: 0");
    if (written < 0 || (size_t)written >= sizeof(snake->text_score_buffer)) {
        SDL_Log("Failed to format initial score text");
        goto fail;
    }

    snake->text_score = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                       snake->text_score_buffer, (size_t)written);
    if (snake->text_score == NULL) {
        SDL_Log("Failed to create score text object: %s", SDL_GetError());
        goto fail;
    }

    if (TTF_SetTextColor(snake->text_score, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set score text color: %s", SDL_GetError());
        goto fail;
    }

    if (reset(snake) == false) {
        SDL_Log("Failed to initialize game state");
        goto fail;
    }

    SDL_Log("Snake game initialized successfully");
    return snake->window.is_running;

fail:
    snake_destroy(snake);
    return false;
}

void snake_destroy(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (snake->text_score != NULL) {
        TTF_DestroyText(snake->text_score);
        snake->text_score = NULL;
    }

    audio_manager_destroy(&snake->audio);
    window_destroy(&snake->window);

    vector2i_set(&snake->position_head, 0, 0);
    vector2i_set(&snake->previous_position_head, 0, 0);
    vector2i_set(&snake->previous_position_tail, 0, 0);

    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_destroy(&snake->array_food);
    dynamic_array_destroy(&snake->array_body);
}

static void handle_movement_keys(snake_t* snake, SDL_Scancode scancode) {
    SDL_assert(snake != NULL);

    if (snake->is_paused == true) {
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
    cell_set_color(snake, &snake->position_head, SNAKE_COLOR_GREEN);

    if (dynamic_array_is_empty(&snake->array_body) == true) {
        // Snake has no array_body, so clear the previous head position.
        cell_set_color(snake, &snake->previous_position_head, SNAKE_COLOR_BLACK);
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

            cell_set_color(snake, current_body_position, SNAKE_COLOR_DARK_GREEN);
            cell_set_color(snake, &snake->previous_position_tail, SNAKE_COLOR_BLACK);
        }
    }
}

bool test_body_collision(snake_t* snake) {
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
                cell_set_color(snake, &new_food_position, SNAKE_COLOR_RED);
            }

            return true;
        }
    }

    return false;
}

void snake_update_fixed(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (snake->is_paused == true) {
        return;
    }

    move_head_and_body(snake);

    if (test_body_collision(snake) == true) {
        SDL_Log("Collision detected! Score: %zu", snake->array_body.size);
        // Restart the game if the snake collides with its own array_body.
        if (reset(snake) == false) {
            SDL_Log("Failed to reset game after collision");
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

        if (update_score_text(snake) == false) {
            snake->window.is_running = false;
        }
    }
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
                snake->is_paused = !snake->is_paused;
            }

            handle_movement_keys(snake, event.key.scancode);
        }
    }
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
            switch (cell->color) {
                case SNAKE_COLOR_BLACK:
                    SDL_SetRenderDrawColor(snake->window.sdl_renderer, 0, 0, 0, 255);
                    break;
                case SNAKE_COLOR_GRAY:
                    SDL_SetRenderDrawColor(snake->window.sdl_renderer, 50, 50, 50, 255);
                    break;
                case SNAKE_COLOR_GREEN:
                    SDL_SetRenderDrawColor(snake->window.sdl_renderer, 0, 255, 0, 255);
                    break;
                case SNAKE_COLOR_DARK_GREEN:
                    SDL_SetRenderDrawColor(snake->window.sdl_renderer, 0, 180, 0, 255);
                    break;
                case SNAKE_COLOR_RED:
                    SDL_SetRenderDrawColor(snake->window.sdl_renderer, 255, 0, 0, 255);
                    break;
            }

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
    if (TTF_GetTextSize(snake->text_score, &text_size.x, &text_size.y) == false) {
        SDL_Log("Failed to measure score text: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    // Render the score text at the top center of the screen.
    if (TTF_DrawRendererText(snake->text_score, (float)(screen_size.x - text_size.x) * 0.5f, 10.f) == false) {
        SDL_Log("Failed to render score text: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    SDL_RenderPresent(snake->window.sdl_renderer);
}
