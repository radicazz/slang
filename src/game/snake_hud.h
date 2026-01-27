#ifndef SNAKE_HUD_H
#define SNAKE_HUD_H

#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../modules/window.h"
#include "../modules/config.h"
#include "../utils/vector.h"

typedef struct {
    TTF_Text* text_score;
    char text_score_buffer[32];

    TTF_Text* text_pause;
    char text_pause_buffer[32];

    TTF_Text* text_resume;
    TTF_Text* text_exit_button;

    TTF_Text* text_start_title;
    TTF_Text* text_start_button;
    TTF_Text* text_start_high_score;
    char text_start_high_score_buffer[48];
    TTF_Text* text_options_button;

    TTF_Text* text_game_over_title;
    TTF_Text* text_game_over_score;
    char text_game_over_score_buffer[80];
    TTF_Text* text_restart_button;

    TTF_Text* text_resume_title;
    char text_resume_countdown_buffer[16];
    SDL_Texture* text_resume_countdown_texture;
    vector2i_t text_resume_countdown_size;

    TTF_Text* text_options_title;
    TTF_Text* text_options_volume_label;
    TTF_Text* text_options_mute_label;
    TTF_Text* text_options_resume_label;
    TTF_Text* text_options_back_button;

    TTF_Text* text_options_volume_value;
    char text_options_volume_value_buffer[16];
    TTF_Text* text_options_resume_value;
    char text_options_resume_value_buffer[8];
} snake_hud_t;

bool snake_hud_create(snake_hud_t* hud, window_t* window, game_config_t* config);
void snake_hud_destroy(snake_hud_t* hud);

bool snake_hud_update_score(snake_hud_t* hud, size_t score);
bool snake_hud_update_pause(snake_hud_t* hud, size_t score);
bool snake_hud_update_game_over(snake_hud_t* hud, size_t score, size_t high_score);
bool snake_hud_update_resume_countdown(snake_hud_t* hud, window_t* window, int seconds);
bool snake_hud_update_start_high_score(snake_hud_t* hud, window_t* window, size_t high_score);
bool snake_hud_update_options_volume(snake_hud_t* hud, window_t* window, float volume);
bool snake_hud_update_options_resume_delay(snake_hud_t* hud, window_t* window, int resume_delay_seconds);

#endif  // SNAKE_HUD_H
