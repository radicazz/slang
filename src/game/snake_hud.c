#include "snake_hud.h"

#include <stdio.h>
#include <string.h>
#include <SDL3/SDL_log.h>

static bool create_resume_countdown_text(snake_hud_t* hud, window_t* window, size_t length) {
    SDL_assert(hud != NULL);
    SDL_assert(window != NULL);

    SDL_Log("create_resume_countdown_text: start");

    if (hud->text_resume_countdown_texture != NULL) {
        SDL_Log("create_resume_countdown_text: destroying old texture");
        SDL_DestroyTexture(hud->text_resume_countdown_texture);
        hud->text_resume_countdown_texture = NULL;
    }
    hud->text_resume_countdown_size.x = 0;
    hud->text_resume_countdown_size.y = 0;

    SDL_Log("create_resume_countdown_text: rendering surface");
    SDL_Surface* surface =
        TTF_RenderText_Blended(window->ttf_font_default, hud->text_resume_countdown_buffer, length,
                               (SDL_Color){255, 255, 255, 255});
    if (surface == NULL) {
        SDL_Log("Failed to render resume countdown surface: %s", SDL_GetError());
        return false;
    }

    SDL_Log("create_resume_countdown_text: creating texture from surface");
    hud->text_resume_countdown_texture = SDL_CreateTextureFromSurface(window->sdl_renderer, surface);
    hud->text_resume_countdown_size.x = surface->w;
    hud->text_resume_countdown_size.y = surface->h;
    SDL_DestroySurface(surface);
    if (hud->text_resume_countdown_texture == NULL) {
        SDL_Log("Failed to create resume countdown texture: %s", SDL_GetError());
        return false;
    }

    SDL_Log("create_resume_countdown_text: setting blend mode");
    if (SDL_SetTextureBlendMode(hud->text_resume_countdown_texture, SDL_BLENDMODE_BLEND) == false) {
        SDL_Log("Failed to set resume countdown texture blend mode: %s", SDL_GetError());
        SDL_DestroyTexture(hud->text_resume_countdown_texture);
        hud->text_resume_countdown_texture = NULL;
        hud->text_resume_countdown_size.x = 0;
        hud->text_resume_countdown_size.y = 0;
        return false;
    }

    SDL_Log("create_resume_countdown_text: done");
    return true;
}

bool snake_hud_create(snake_hud_t* hud, window_t* window, game_config_t* config) {
    SDL_assert(hud != NULL);
    SDL_assert(window != NULL);
    SDL_assert(config != NULL);

    memset(hud, 0, sizeof(*hud));

    const int written = snprintf(hud->text_score_buffer, sizeof(hud->text_score_buffer), "Score: 0");
    if (written < 0 || (size_t)written >= sizeof(hud->text_score_buffer)) {
        SDL_Log("Failed to format initial score text");
        return false;
    }

    hud->text_score = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                     hud->text_score_buffer, (size_t)written);
    if (hud->text_score == NULL) {
        SDL_Log("Failed to create score text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_score, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set score text color: %s", SDL_GetError());
        return false;
    }

    const int pause_written = snprintf(hud->text_pause_buffer, sizeof(hud->text_pause_buffer), "Paused: 0");
    if (pause_written < 0 || (size_t)pause_written >= sizeof(hud->text_pause_buffer)) {
        SDL_Log("Failed to format initial pause text");
        return false;
    }

    hud->text_pause = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                     hud->text_pause_buffer, (size_t)pause_written);
    if (hud->text_pause == NULL) {
        SDL_Log("Failed to create pause text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_pause, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set pause text color: %s", SDL_GetError());
        return false;
    }

    const char* resume_label = "Resume";
    hud->text_resume = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default, resume_label,
                                      SDL_strlen(resume_label));
    if (hud->text_resume == NULL) {
        SDL_Log("Failed to create resume text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_resume, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set resume text color: %s", SDL_GetError());
        return false;
    }

    const char* start_title = "Start Game";
    hud->text_start_title = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default, start_title,
                                           SDL_strlen(start_title));
    if (hud->text_start_title == NULL) {
        SDL_Log("Failed to create start title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_start_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set start title text color: %s", SDL_GetError());
        return false;
    }

    const char* start_label = "Start";
    hud->text_start_button = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                            start_label, SDL_strlen(start_label));
    if (hud->text_start_button == NULL) {
        SDL_Log("Failed to create start button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_start_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set start button text color: %s", SDL_GetError());
        return false;
    }

    const char* options_label = "Options";
    hud->text_options_button = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                              options_label, SDL_strlen(options_label));
    if (hud->text_options_button == NULL) {
        SDL_Log("Failed to create options button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_options_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set options button text color: %s", SDL_GetError());
        return false;
    }

    if (snake_hud_update_start_high_score(hud, window, config->high_score) == false) {
        return false;
    }

    const char* game_over_title = "Game Over";
    hud->text_game_over_title = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                               game_over_title, SDL_strlen(game_over_title));
    if (hud->text_game_over_title == NULL) {
        SDL_Log("Failed to create game over title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_game_over_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set game over title text color: %s", SDL_GetError());
        return false;
    }

    const int game_over_written =
        snprintf(hud->text_game_over_score_buffer, sizeof(hud->text_game_over_score_buffer), "Final Score: 0");
    if (game_over_written < 0 || (size_t)game_over_written >= sizeof(hud->text_game_over_score_buffer)) {
        SDL_Log("Failed to format initial game over score text");
        return false;
    }

    hud->text_game_over_score = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                               hud->text_game_over_score_buffer, (size_t)game_over_written);
    if (hud->text_game_over_score == NULL) {
        SDL_Log("Failed to create game over score text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_game_over_score, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set game over score text color: %s", SDL_GetError());
        return false;
    }

    if (snake_hud_update_game_over(hud, 0, config->high_score) == false) {
        return false;
    }

    const char* restart_label = "Restart";
    hud->text_restart_button = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                              restart_label, SDL_strlen(restart_label));
    if (hud->text_restart_button == NULL) {
        SDL_Log("Failed to create restart button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_restart_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set restart button text color: %s", SDL_GetError());
        return false;
    }

    const char* resume_title = "Resuming";
    hud->text_resume_title = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                            resume_title, SDL_strlen(resume_title));
    if (hud->text_resume_title == NULL) {
        SDL_Log("Failed to create resume title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_resume_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set resume title text color: %s", SDL_GetError());
        return false;
    }

    const int resume_written =
        snprintf(hud->text_resume_countdown_buffer, sizeof(hud->text_resume_countdown_buffer), "3");
    if (resume_written < 0 || (size_t)resume_written >= sizeof(hud->text_resume_countdown_buffer)) {
        SDL_Log("Failed to format initial resume countdown text");
        return false;
    }

    if (create_resume_countdown_text(hud, window, (size_t)resume_written) == false) {
        return false;
    }

    const char* options_title = "Options";
    hud->text_options_title = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                             options_title, SDL_strlen(options_title));
    if (hud->text_options_title == NULL) {
        SDL_Log("Failed to create options title text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_options_title, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set options title text color: %s", SDL_GetError());
        return false;
    }

    const char* volume_label = "Volume";
    hud->text_options_volume_label = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                                    volume_label, SDL_strlen(volume_label));
    if (hud->text_options_volume_label == NULL) {
        SDL_Log("Failed to create volume label text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_options_volume_label, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set volume label text color: %s", SDL_GetError());
        return false;
    }

    const char* mute_label = "Mute";
    hud->text_options_mute_label = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                                  mute_label, SDL_strlen(mute_label));
    if (hud->text_options_mute_label == NULL) {
        SDL_Log("Failed to create mute label text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_options_mute_label, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set mute label text color: %s", SDL_GetError());
        return false;
    }

    const char* resume_delay_label = "Resume Delay";
    hud->text_options_resume_label =
        TTF_CreateText(window->ttf_text_engine, window->ttf_font_default, resume_delay_label,
                       SDL_strlen(resume_delay_label));
    if (hud->text_options_resume_label == NULL) {
        SDL_Log("Failed to create resume delay label text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_options_resume_label, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set resume delay label text color: %s", SDL_GetError());
        return false;
    }

    const char* back_label = "Back";
    hud->text_options_back_button = TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                                                   back_label, SDL_strlen(back_label));
    if (hud->text_options_back_button == NULL) {
        SDL_Log("Failed to create back button text object: %s", SDL_GetError());
        return false;
    }

    if (TTF_SetTextColor(hud->text_options_back_button, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set back button text color: %s", SDL_GetError());
        return false;
    }

    if (snake_hud_update_options_volume(hud, window, config->volume) == false) {
        return false;
    }

    if (snake_hud_update_options_resume_delay(hud, window, config->resume_delay_seconds) == false) {
        return false;
    }

    return true;
}

void snake_hud_destroy(snake_hud_t* hud) {
    SDL_assert(hud != NULL);

    if (hud->text_score != NULL) {
        TTF_DestroyText(hud->text_score);
        hud->text_score = NULL;
    }

    if (hud->text_pause != NULL) {
        TTF_DestroyText(hud->text_pause);
        hud->text_pause = NULL;
    }

    if (hud->text_resume != NULL) {
        TTF_DestroyText(hud->text_resume);
        hud->text_resume = NULL;
    }

    if (hud->text_start_title != NULL) {
        TTF_DestroyText(hud->text_start_title);
        hud->text_start_title = NULL;
    }

    if (hud->text_start_button != NULL) {
        TTF_DestroyText(hud->text_start_button);
        hud->text_start_button = NULL;
    }

    if (hud->text_start_high_score != NULL) {
        TTF_DestroyText(hud->text_start_high_score);
        hud->text_start_high_score = NULL;
    }

    if (hud->text_options_button != NULL) {
        TTF_DestroyText(hud->text_options_button);
        hud->text_options_button = NULL;
    }

    if (hud->text_game_over_title != NULL) {
        TTF_DestroyText(hud->text_game_over_title);
        hud->text_game_over_title = NULL;
    }

    if (hud->text_game_over_score != NULL) {
        TTF_DestroyText(hud->text_game_over_score);
        hud->text_game_over_score = NULL;
    }

    if (hud->text_restart_button != NULL) {
        TTF_DestroyText(hud->text_restart_button);
        hud->text_restart_button = NULL;
    }

    if (hud->text_resume_title != NULL) {
        TTF_DestroyText(hud->text_resume_title);
        hud->text_resume_title = NULL;
    }

    if (hud->text_resume_countdown_texture != NULL) {
        SDL_DestroyTexture(hud->text_resume_countdown_texture);
        hud->text_resume_countdown_texture = NULL;
    }
    hud->text_resume_countdown_size.x = 0;
    hud->text_resume_countdown_size.y = 0;

    if (hud->text_options_title != NULL) {
        TTF_DestroyText(hud->text_options_title);
        hud->text_options_title = NULL;
    }

    if (hud->text_options_volume_label != NULL) {
        TTF_DestroyText(hud->text_options_volume_label);
        hud->text_options_volume_label = NULL;
    }

    if (hud->text_options_mute_label != NULL) {
        TTF_DestroyText(hud->text_options_mute_label);
        hud->text_options_mute_label = NULL;
    }

    if (hud->text_options_resume_label != NULL) {
        TTF_DestroyText(hud->text_options_resume_label);
        hud->text_options_resume_label = NULL;
    }

    if (hud->text_options_back_button != NULL) {
        TTF_DestroyText(hud->text_options_back_button);
        hud->text_options_back_button = NULL;
    }

    if (hud->text_options_volume_value != NULL) {
        TTF_DestroyText(hud->text_options_volume_value);
        hud->text_options_volume_value = NULL;
    }

    if (hud->text_options_resume_value != NULL) {
        TTF_DestroyText(hud->text_options_resume_value);
        hud->text_options_resume_value = NULL;
    }
}

bool snake_hud_update_score(snake_hud_t* hud, size_t score) {
    SDL_assert(hud != NULL);
    SDL_assert(hud->text_score != NULL);

    const int written = snprintf(hud->text_score_buffer, sizeof(hud->text_score_buffer), "Score: %zu", score);
    if (written < 0 || (size_t)written >= sizeof(hud->text_score_buffer)) {
        SDL_Log("Failed to format score text.");
        return false;
    }

    if (TTF_SetTextString(hud->text_score, hud->text_score_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update score text: %s", SDL_GetError());
        return false;
    }

    return snake_hud_update_pause(hud, score);
}

bool snake_hud_update_pause(snake_hud_t* hud, size_t score) {
    SDL_assert(hud != NULL);
    SDL_assert(hud->text_pause != NULL);

    const int written = snprintf(hud->text_pause_buffer, sizeof(hud->text_pause_buffer), "Paused: %zu", score);
    if (written < 0 || (size_t)written >= sizeof(hud->text_pause_buffer)) {
        SDL_Log("Failed to format pause text.");
        return false;
    }

    if (TTF_SetTextString(hud->text_pause, hud->text_pause_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update pause text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_hud_update_game_over(snake_hud_t* hud, size_t score, size_t high_score) {
    SDL_assert(hud != NULL);
    SDL_assert(hud->text_game_over_score != NULL);

    const int written =
        snprintf(hud->text_game_over_score_buffer, sizeof(hud->text_game_over_score_buffer),
                 "Final Score: %zu | High Score: %zu", score, high_score);
    if (written < 0 || (size_t)written >= sizeof(hud->text_game_over_score_buffer)) {
        SDL_Log("Failed to format game over score text.");
        return false;
    }

    if (TTF_SetTextString(hud->text_game_over_score, hud->text_game_over_score_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update game over score text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_hud_update_start_high_score(snake_hud_t* hud, window_t* window, size_t high_score) {
    SDL_assert(hud != NULL);
    SDL_assert(window != NULL);

    const int written = snprintf(hud->text_start_high_score_buffer, sizeof(hud->text_start_high_score_buffer),
                                 "High Score: %zu", high_score);
    if (written < 0 || (size_t)written >= sizeof(hud->text_start_high_score_buffer)) {
        SDL_Log("Failed to format start high score text.");
        return false;
    }

    if (hud->text_start_high_score == NULL) {
        hud->text_start_high_score =
            TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                           hud->text_start_high_score_buffer, (size_t)written);
        if (hud->text_start_high_score == NULL) {
            SDL_Log("Failed to create start high score text object: %s", SDL_GetError());
            return false;
        }
        if (TTF_SetTextColor(hud->text_start_high_score, 255, 255, 255, 255) == false) {
            SDL_Log("Failed to set start high score text color: %s", SDL_GetError());
            return false;
        }
    }

    if (TTF_SetTextString(hud->text_start_high_score, hud->text_start_high_score_buffer, (size_t)written) ==
        false) {
        SDL_Log("Failed to update start high score text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_hud_update_options_volume(snake_hud_t* hud, window_t* window, float volume) {
    SDL_assert(hud != NULL);
    SDL_assert(window != NULL);

    const int written = snprintf(hud->text_options_volume_value_buffer,
                                 sizeof(hud->text_options_volume_value_buffer), "%.2f", volume);
    if (written < 0 || (size_t)written >= sizeof(hud->text_options_volume_value_buffer)) {
        SDL_Log("Failed to format options volume value.");
        return false;
    }

    if (hud->text_options_volume_value == NULL) {
        hud->text_options_volume_value =
            TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                           hud->text_options_volume_value_buffer, (size_t)written);
        if (hud->text_options_volume_value == NULL) {
            SDL_Log("Failed to create options volume value text: %s", SDL_GetError());
            return false;
        }
        if (TTF_SetTextColor(hud->text_options_volume_value, 255, 255, 255, 255) == false) {
            SDL_Log("Failed to set options volume value text color: %s", SDL_GetError());
            return false;
        }
    }

    if (TTF_SetTextString(hud->text_options_volume_value, hud->text_options_volume_value_buffer, (size_t)written) ==
        false) {
        SDL_Log("Failed to update options volume value text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_hud_update_options_resume_delay(snake_hud_t* hud, window_t* window, int resume_delay_seconds) {
    SDL_assert(hud != NULL);
    SDL_assert(window != NULL);

    const int written = snprintf(hud->text_options_resume_value_buffer,
                                 sizeof(hud->text_options_resume_value_buffer), "%d",
                                 resume_delay_seconds);
    if (written < 0 || (size_t)written >= sizeof(hud->text_options_resume_value_buffer)) {
        SDL_Log("Failed to format options resume delay.");
        return false;
    }

    if (hud->text_options_resume_value == NULL) {
        hud->text_options_resume_value =
            TTF_CreateText(window->ttf_text_engine, window->ttf_font_default,
                           hud->text_options_resume_value_buffer, (size_t)written);
        if (hud->text_options_resume_value == NULL) {
            SDL_Log("Failed to create resume delay value text: %s", SDL_GetError());
            return false;
        }
        if (TTF_SetTextColor(hud->text_options_resume_value, 255, 255, 255, 255) == false) {
            SDL_Log("Failed to set resume delay value text color: %s", SDL_GetError());
            return false;
        }
    }

    if (TTF_SetTextString(hud->text_options_resume_value, hud->text_options_resume_value_buffer, (size_t)written) ==
        false) {
        SDL_Log("Failed to update resume delay value text: %s", SDL_GetError());
        return false;
    }

    return true;
}

bool snake_hud_update_resume_countdown(snake_hud_t* hud, window_t* window, int seconds) {
    SDL_assert(hud != NULL);
    SDL_assert(window != NULL);

    if (seconds < 0) {
        seconds = 0;
    }

    const int written = snprintf(hud->text_resume_countdown_buffer, sizeof(hud->text_resume_countdown_buffer), "%d", seconds);
    if (written < 0 || (size_t)written >= sizeof(hud->text_resume_countdown_buffer)) {
        SDL_Log("Failed to format resume countdown text.");
        return false;
    }

    return create_resume_countdown_text(hud, window, (size_t)written);
}
