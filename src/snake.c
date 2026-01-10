#include "snake.h"

#include <stdint.h>
#include <string.h>
#include <SDL3/SDL_log.h>

#include "game/snake_state.h"
#include "game/snake_text.h"

bool snake_create(snake_t* snake, const char* title) {
    SDL_assert(snake != NULL);
    SDL_assert(title != NULL);

    SDL_Log("Initializing snake game");

    memset(snake, 0, sizeof(*snake));

    if (window_create(&snake->window, title, WINDOW_WIDTH, WINDOW_HEIGHT) == false) {
        SDL_Log("Failed to create game window");
        return false;
    }

    window_frame_init(&snake->window_frame);

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

    if (snake_text_create(snake) == false) {
        SDL_Log("Failed to initialize text resources");
        goto fail;
    }

    if (snake_state_reset(snake) == false) {
        SDL_Log("Failed to initialize game state");
        goto fail;
    }

    snake->state = SNAKE_STATE_START;

    SDL_Log("Snake game initialized successfully");
    return snake->window.is_running;

fail:
    snake_destroy(snake);
    return false;
}

void snake_destroy(snake_t* snake) {
    SDL_assert(snake != NULL);

    snake_text_destroy(snake);

    audio_manager_destroy(&snake->audio);
    window_destroy(&snake->window);

    vector2i_set(&snake->position_head, 0, 0);
    vector2i_set(&snake->previous_position_head, 0, 0);
    vector2i_set(&snake->previous_position_tail, 0, 0);

    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_destroy(&snake->array_food);
    dynamic_array_destroy(&snake->array_body);
}
