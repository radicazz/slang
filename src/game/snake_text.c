#include "snake_text.h"

#include <stdio.h>
#include <SDL3/SDL_log.h>

static bool create_resume_countdown_text(snake_t* snake, size_t length) {
    SDL_assert(snake != NULL);

    if (snake->text_resume_countdown != NULL) {
        TTF_DestroyText(snake->text_resume_countdown);
        snake->text_resume_countdown = NULL;
    }
    if (snake->text_resume_countdown_texture != NULL) {
        SDL_DestroyTexture(snake->text_resume_countdown_texture);
        snake->text_resume_countdown_texture = NULL;
    }
    snake->text_resume_countdown_size.x = 0;
    snake->text_resume_countdown_size.y = 0;

    snake->text_resume_countdown = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                  snake->text_resume_countdown_buffer, length);
    if (snake->text_resume_countdown == NULL) {
        SDL_Log("Failed to create resume countdown text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_resume_countdown, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set resume countdown text color: %s", SDL_GetError());
        return false;
    }

    SDL_Surface* surface =
        TTF_RenderText_Blended(snake->window.ttf_font_default, snake->text_resume_countdown_buffer, length,
                               (SDL_Color){255, 255, 255, 255});
    if (surface == NULL) {
        SDL_Log("Failed to render resume countdown surface: %s", SDL_GetError());
        return false;
    }

    snake->text_resume_countdown_texture = SDL_CreateTextureFromSurface(snake->window.sdl_renderer, surface);
    snake->text_resume_countdown_size.x = surface->w;
    snake->text_resume_countdown_size.y = surface->h;
    SDL_DestroySurface(surface);
    if (snake->text_resume_countdown_texture == NULL) {
        SDL_Log("Failed to create resume countdown texture: %s", SDL_GetError());
        return false;
    }

    if (SDL_SetTextureBlendMode(snake->text_resume_countdown_texture, SDL_BLENDMODE_BLEND) == false) {
        SDL_Log("Failed to set resume countdown texture blend mode: %s", SDL_GetError());
        SDL_DestroyTexture(snake->text_resume_countdown_texture);
        snake->text_resume_countdown_texture = NULL;
        snake->text_resume_countdown_size.x = 0;
        snake->text_resume_countdown_size.y = 0;
        return false;
    }

    return true;
}

bool snake_text_create(snake_t* snake) {
    SDL_assert(snake != NULL);

    const int written = snprintf(snake->text_score_buffer, sizeof(snake->text_score_buffer), "Score: 0");
    if (written < 0 || (size_t)written >= sizeof(snake->text_score_buffer)) {
        SDL_Log("Failed to format initial score text");
        return false;
    }

    snake->text_score = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                       snake->text_score_buffer, (size_t)written);
    if (snake->text_score == NULL) {
        SDL_Log("Failed to create score text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_score, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set score text color: %s", SDL_GetError());
        return false;
    }

    const int pause_written = snprintf(snake->text_pause_buffer, sizeof(snake->text_pause_buffer), "Paused: 0");
    if (pause_written < 0 || (size_t)pause_written >= sizeof(snake->text_pause_buffer)) {
        SDL_Log("Failed to format initial pause text");
        return false;
    }

    snake->text_pause = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                       snake->text_pause_buffer, (size_t)pause_written);
    if (snake->text_pause == NULL) {
        SDL_Log("Failed to create pause text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_pause, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set pause text color: %s", SDL_GetError());
        return false;
    }

    const char* resume_label = "Resume";
    snake->text_resume = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default, resume_label,
                                        SDL_strlen(resume_label));
    if (snake->text_resume == NULL) {
        SDL_Log("Failed to create resume text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_resume, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set resume text color: %s", SDL_GetError());
        return false;
    }

    const char* start_title = "Start Game";
    snake->text_start_title = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default, start_title,
                                             SDL_strlen(start_title));
    if (snake->text_start_title == NULL) {
        SDL_Log("Failed to create start title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_start_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set start title text color: %s", SDL_GetError());
        return false;
    }

    const char* start_label = "Start";
    snake->text_start_button = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                              start_label, SDL_strlen(start_label));
    if (snake->text_start_button == NULL) {
        SDL_Log("Failed to create start button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_start_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set start button text color: %s", SDL_GetError());
        return false;
    }

    const char* options_label = "Options";
    snake->text_options_button = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                options_label, SDL_strlen(options_label));
    if (snake->text_options_button == NULL) {
        SDL_Log("Failed to create options button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_options_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set options button text color: %s", SDL_GetError());
        return false;
    }

    if (snake_text_update_start_high_score(snake) == false) {
        return false;
    }

    const char* game_over_title = "Game Over";
    snake->text_game_over_title = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                 game_over_title, SDL_strlen(game_over_title));
    if (snake->text_game_over_title == NULL) {
        SDL_Log("Failed to create game over title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_game_over_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set game over title text color: %s", SDL_GetError());
        return false;
    }

    const int game_over_written =
        snprintf(snake->text_game_over_score_buffer, sizeof(snake->text_game_over_score_buffer), "Final Score: 0");
    if (game_over_written < 0 || (size_t)game_over_written >= sizeof(snake->text_game_over_score_buffer)) {
        SDL_Log("Failed to format initial game over score text");
        return false;
    }

    snake->text_game_over_score = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                 snake->text_game_over_score_buffer, (size_t)game_over_written);
    if (snake->text_game_over_score == NULL) {
        SDL_Log("Failed to create game over score text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_game_over_score, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set game over score text color: %s", SDL_GetError());
        return false;
    }

    if (snake_text_update_high_score(snake) == false) {
        return false;
    }

    const char* restart_label = "Restart";
    snake->text_restart_button = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                restart_label, SDL_strlen(restart_label));
    if (snake->text_restart_button == NULL) {
        SDL_Log("Failed to create restart button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_restart_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set restart button text color: %s", SDL_GetError());
        return false;
    }

    const char* resume_title = "Resuming";
    snake->text_resume_title = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                              resume_title, SDL_strlen(resume_title));
    if (snake->text_resume_title == NULL) {
        SDL_Log("Failed to create resume title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_resume_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set resume title text color: %s", SDL_GetError());
        return false;
    }

    const int resume_written =
        snprintf(snake->text_resume_countdown_buffer, sizeof(snake->text_resume_countdown_buffer), "3");
    if (resume_written < 0 || (size_t)resume_written >= sizeof(snake->text_resume_countdown_buffer)) {
        SDL_Log("Failed to format initial resume countdown text");
        return false;
    }

    if (create_resume_countdown_text(snake, (size_t)resume_written) == false) {
        return false;
    }

    const char* options_title = "Options";
    snake->text_options_title = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                               options_title, SDL_strlen(options_title));
    if (snake->text_options_title == NULL) {
        SDL_Log("Failed to create options title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_options_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set options title text color: %s", SDL_GetError());
        return false;
    }

    const char* volume_label = "Volume";
    snake->text_options_volume_label = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                      volume_label, SDL_strlen(volume_label));
    if (snake->text_options_volume_label == NULL) {
        SDL_Log("Failed to create volume label text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_options_volume_label, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set volume label text color: %s", SDL_GetError());
        return false;
    }

    const char* mute_label = "Mute";
    snake->text_options_mute_label = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                    mute_label, SDL_strlen(mute_label));
    if (snake->text_options_mute_label == NULL) {
        SDL_Log("Failed to create mute label text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_options_mute_label, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set mute label text color: %s", SDL_GetError());
        return false;
    }

    const char* back_label = "Back";
    snake->text_options_back_button = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                                     back_label, SDL_strlen(back_label));
    if (snake->text_options_back_button == NULL) {
        SDL_Log("Failed to create back button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(snake->text_options_back_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set back button text color: %s", SDL_GetError());
        return false;
    }

    if (snake_text_update_options_labels(snake) == false) {
        return false;
    }

    return true;
}

void snake_text_destroy(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (snake->text_score != NULL) {
        TTF_DestroyText(snake->text_score);
        snake->text_score = NULL;
    }

    if (snake->text_pause != NULL) {
        TTF_DestroyText(snake->text_pause);
        snake->text_pause = NULL;
    }

    if (snake->text_resume != NULL) {
        TTF_DestroyText(snake->text_resume);
        snake->text_resume = NULL;
    }

    if (snake->text_start_title != NULL) {
        TTF_DestroyText(snake->text_start_title);
        snake->text_start_title = NULL;
    }

    if (snake->text_start_button != NULL) {
        TTF_DestroyText(snake->text_start_button);
        snake->text_start_button = NULL;
    }

    if (snake->text_start_high_score != NULL) {
        TTF_DestroyText(snake->text_start_high_score);
        snake->text_start_high_score = NULL;
    }

    if (snake->text_options_button != NULL) {
        TTF_DestroyText(snake->text_options_button);
        snake->text_options_button = NULL;
    }

    if (snake->text_game_over_title != NULL) {
        TTF_DestroyText(snake->text_game_over_title);
        snake->text_game_over_title = NULL;
    }

    if (snake->text_game_over_score != NULL) {
        TTF_DestroyText(snake->text_game_over_score);
        snake->text_game_over_score = NULL;
    }

    if (snake->text_restart_button != NULL) {
        TTF_DestroyText(snake->text_restart_button);
        snake->text_restart_button = NULL;
    }

    if (snake->text_resume_title != NULL) {
        TTF_DestroyText(snake->text_resume_title);
        snake->text_resume_title = NULL;
    }

    if (snake->text_resume_countdown != NULL) {
        TTF_DestroyText(snake->text_resume_countdown);
        snake->text_resume_countdown = NULL;
    }
    if (snake->text_resume_countdown_texture != NULL) {
        SDL_DestroyTexture(snake->text_resume_countdown_texture);
        snake->text_resume_countdown_texture = NULL;
    }
    snake->text_resume_countdown_size.x = 0;
    snake->text_resume_countdown_size.y = 0;

    if (snake->text_options_title != NULL) {
        TTF_DestroyText(snake->text_options_title);
        snake->text_options_title = NULL;
    }

    if (snake->text_options_volume_label != NULL) {
        TTF_DestroyText(snake->text_options_volume_label);
        snake->text_options_volume_label = NULL;
    }

    if (snake->text_options_mute_label != NULL) {
        TTF_DestroyText(snake->text_options_mute_label);
        snake->text_options_mute_label = NULL;
    }

    if (snake->text_options_back_button != NULL) {
        TTF_DestroyText(snake->text_options_back_button);
        snake->text_options_back_button = NULL;
    }

    if (snake->text_options_volume_value != NULL) {
        TTF_DestroyText(snake->text_options_volume_value);
        snake->text_options_volume_value = NULL;
    }
}

bool snake_text_update_score(snake_t* snake) {
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

    return snake_text_update_pause(snake);
}

bool snake_text_update_pause(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_pause != NULL);

    const int written =
        snprintf(snake->text_pause_buffer, sizeof(snake->text_pause_buffer), "Paused: %zu", snake->array_body.size);
    if (written < 0 || (size_t)written >= sizeof(snake->text_pause_buffer)) {
        SDL_Log("Failed to format pause text.");
        return false;
    }

    if (TTF_SetTextString(snake->text_pause, snake->text_pause_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update pause text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_text_update_game_over(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_game_over_score != NULL);

    const int written =
        snprintf(snake->text_game_over_score_buffer, sizeof(snake->text_game_over_score_buffer),
                 "Final Score: %zu | High Score: %zu", snake->array_body.size, snake->config.high_score);
    if (written < 0 || (size_t)written >= sizeof(snake->text_game_over_score_buffer)) {
        SDL_Log("Failed to format game over score text.");
        return false;
    }

    if (TTF_SetTextString(snake->text_game_over_score, snake->text_game_over_score_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update game over score text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_text_update_high_score(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_game_over_score != NULL);

    return snake_text_update_game_over(snake);
}

bool snake_text_update_start_high_score(snake_t* snake) {
    SDL_assert(snake != NULL);

    const int written = snprintf(snake->text_start_high_score_buffer, sizeof(snake->text_start_high_score_buffer),
                                 "High Score: %zu", snake->config.high_score);
    if (written < 0 || (size_t)written >= sizeof(snake->text_start_high_score_buffer)) {
        SDL_Log("Failed to format start high score text.");
        return false;
    }

    if (snake->text_start_high_score == NULL) {
        snake->text_start_high_score =
            TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                           snake->text_start_high_score_buffer, (size_t)written);
        if (snake->text_start_high_score == NULL) {
            SDL_Log("Failed to create start high score text object: %s", SDL_GetError());
            return false;
        }
        if (TTF_SetTextColor(snake->text_start_high_score, 255, 255, 255, 255) == false) {
            SDL_Log("Failed to set start high score text color: %s", SDL_GetError());
            return false;
        }
    }

    if (TTF_SetTextString(snake->text_start_high_score, snake->text_start_high_score_buffer, (size_t)written) ==
        false) {
        SDL_Log("Failed to update start high score text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_text_update_options_labels(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_options_volume_label != NULL);
    SDL_assert(snake->text_options_mute_label != NULL);
    SDL_assert(snake->text_options_title != NULL);
    SDL_assert(snake->text_options_back_button != NULL);

    return snake_text_update_options_volume(snake);
}

bool snake_text_update_options_volume(snake_t* snake) {
    SDL_assert(snake != NULL);

    const int written = snprintf(snake->text_options_volume_value_buffer,
                                 sizeof(snake->text_options_volume_value_buffer), "%.2f", snake->config.volume);
    if (written < 0 || (size_t)written >= sizeof(snake->text_options_volume_value_buffer)) {
        SDL_Log("Failed to format options volume value.");
        return false;
    }

    if (snake->text_options_volume_value == NULL) {
        snake->text_options_volume_value =
            TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                           snake->text_options_volume_value_buffer, (size_t)written);
        if (snake->text_options_volume_value == NULL) {
            SDL_Log("Failed to create options volume value text: %s", SDL_GetError());
            return false;
        }
        if (TTF_SetTextColor(snake->text_options_volume_value, 255, 255, 255, 255) == false) {
            SDL_Log("Failed to set options volume value text color: %s", SDL_GetError());
            return false;
        }
    }

    if (TTF_SetTextString(snake->text_options_volume_value, snake->text_options_volume_value_buffer, (size_t)written) ==
        false) {
        SDL_Log("Failed to update options volume value text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_text_update_resume_countdown(snake_t* snake, int seconds) {
    SDL_assert(snake != NULL);

    if (seconds < 0) {
        seconds = 0;
    }

    const int written =
        snprintf(snake->text_resume_countdown_buffer, sizeof(snake->text_resume_countdown_buffer), "%d", seconds);
    if (written < 0 || (size_t)written >= sizeof(snake->text_resume_countdown_buffer)) {
        SDL_Log("Failed to format resume countdown text.");
        return false;
    }

    return create_resume_countdown_text(snake, (size_t)written);
}
