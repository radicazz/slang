#include "snake.h"

#include <stdio.h>
#include <assert.h>

static void handle_sdl_error(const char* message) {
    const char* error = SDL_GetError();
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", message, error);
    SDL_ClearError();
}

static void snake_reset(snake_t* snake) {
    assert(snake != NULL);

    // Generate random head position.
    snake->head_position.x = random_int(1, SNAKE_WINDOW_X - 1);
    snake->head_position.y = random_int(1, SNAKE_WINDOW_Y - 1);

    snake->previous_head_position = snake->head_position;

    snake->current_direction = SNAKE_DIRECTION_UP;

    // Initialize all cells.
    for (int x = 0; x < SNAKE_WINDOW_X; ++x) {
        for (int y = 0; y < SNAKE_WINDOW_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            // Set the border cells to gray, the head cell to green, and the rest to black.
            if (x == 0 || x == SNAKE_WINDOW_X - 1 || y == 0 || y == SNAKE_WINDOW_Y - 1) {
                cell->color = SNAKE_COLOR_GRAY;
            } else if (x == snake->head_position.x && y == snake->head_position.y) {
                cell->color = SNAKE_COLOR_GREEN;
            } else {
                cell->color = SNAKE_COLOR_BLACK;
            }
        }
    }
}

bool snake_create(snake_t* snake, const char* title) {
    assert(snake != NULL);
    assert(title != NULL);

    if (SDL_CreateWindowAndRenderer(title, SNAKE_WINDOW_WIDTH, SNAKE_WINDOW_HEIGHT, 0, &snake->window,
                                    &snake->renderer) == false) {
        handle_sdl_error("Failed to create window and renderer");
        return false;
    }

    snake_reset(snake);
    snake->is_running = true;

    return true;
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

    snake->head_position.x = 0;
    snake->head_position.y = 0;

    snake->previous_head_position = snake->head_position;

    snake->current_direction = SNAKE_DIRECTION_UP;

    for (int x = 0; x < SNAKE_WINDOW_X; ++x) {
        for (int y = 0; y < SNAKE_WINDOW_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            cell->color = SNAKE_COLOR_BLACK;
        }
    }
}

static void snake_handle_key_presses(snake_t* snake, SDL_Scancode scancode) {
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

static void snake_move(snake_t* snake) {
    assert(snake != NULL);

    snake->previous_head_position = snake->head_position;

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

    snake->head_position = new_head_position;
}

static void snake_update(snake_t* snake) {
    assert(snake != NULL);

    snake_move(snake);

    snake->cells[snake->head_position.x][snake->head_position.y].color = SNAKE_COLOR_GREEN;
    snake->cells[snake->previous_head_position.x][snake->previous_head_position.y].color = SNAKE_COLOR_BLACK;
}

void snake_handle_events(snake_t* snake) {
    assert(snake != NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            snake->is_running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            snake_handle_key_presses(snake, event.key.scancode);
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
