#include "snake.h"

#include <stdint.h>
#include <string.h>
#include <SDL3/SDL_log.h>

#include "game/snake_state.h"
#include "game/snake_text.h"
#include "modules/config.h"

bool snake_create(snake_t* snake, const char* title) {
    SDL_assert(snake != NULL);
    SDL_assert(title != NULL);

    SDL_Log("Initializing snake game");

    memset(snake, 0, sizeof(*snake));

    if (window_create(&snake->window, title, WINDOW_WIDTH, WINDOW_HEIGHT) == false) {
        SDL_Log("Failed to create game window");
        return false;
    }

    if (config_load(&snake->config) == false) {
        SDL_Log("Warning: Failed to load config, continuing with defaults");
        config_set_defaults(&snake->config);
    }

    if (audio_manager_create(&snake->audio) == false) {
        SDL_Log("Warning: Failed to initialize audio, continuing without sound");
    } else {
        snake_apply_audio_settings(snake);
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
    snake->options_return_state = SNAKE_STATE_START;
    snake->options_dragging_volume = false;
    snake->options_dragging_resume = false;

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

bool snake_apply_audio_settings(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (audio_manager_set_volume(&snake->audio, snake->config.volume) == false) {
        return false;
    }
    if (audio_manager_set_muted(&snake->audio, snake->config.mute) == false) {
        return false;
    }

    return true;
}

bool snake_save_config(snake_t* snake) {
    SDL_assert(snake != NULL);
    return config_save(&snake->config);
}
