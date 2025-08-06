#include "snake.h"

#include <stdio.h>
#include <assert.h>

static void handle_sdl_error(const char* message) {
    const char* error = SDL_GetError();
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s: %s", message, error);
    SDL_ClearError();
}

void snake_init(snake_t* snake) {
    assert(snake != NULL);

    snake->window = NULL;
    snake->renderer = NULL;
    snake->is_running = false;
    snake->head_position = (ivec2_t){0, 0};
    snake->current_direction = SNAKE_DIRECTION_UNDEFINED;
}

bool snake_create(snake_t* snake, const char* title) {
    assert(snake != NULL);
    assert(title != NULL);

    if (SDL_CreateWindowAndRenderer(title, SNAKE_WINDOW_WIDTH, SNAKE_WINDOW_HEIGHT, 0, &snake->window,
                                    &snake->renderer) == false) {
        handle_sdl_error("Failed to create window and renderer");
        return false;
    }

    snake->is_running = true;
    snake->head_position = (ivec2_t){SNAKE_WINDOW_WIDTH / 2, SNAKE_WINDOW_HEIGHT / 2};
    snake->current_direction = random_int(SNAKE_DIRECTION_UP, SNAKE_DIRECTION_RIGHT);

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

    snake->is_running = false;

    snake->head_position = (ivec2_t){0, 0};
    snake->current_direction = SNAKE_DIRECTION_UP;
}

void snake_handle_events(snake_t* snake) {
    assert(snake != NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            snake->is_running = false;
        }
    }
}

void snake_render(snake_t* snake) {
    assert(snake != NULL);

    if (snake->renderer == NULL) {
        handle_sdl_error("Renderer is not initialized");
        return;
    }

    SDL_RenderClear(snake->renderer);

    SDL_SetRenderDrawColor(snake->renderer, 255, 255, 255, 255);
    SDL_FRect rect;
    rect.x = 100.0f;
    rect.y = 100.0f;
    rect.w = 50.0f;
    rect.h = 50.0f;
    SDL_RenderFillRect(snake->renderer, &rect);

    SDL_SetRenderDrawColor(snake->renderer, 0, 0, 0, 255);
    SDL_RenderPresent(snake->renderer);
}
