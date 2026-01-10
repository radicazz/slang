#ifndef SNAKE_TEXT_H
#define SNAKE_TEXT_H

#include "../snake.h"

bool snake_text_create(snake_t* snake);
void snake_text_destroy(snake_t* snake);

bool snake_text_update_score(snake_t* snake);
bool snake_text_update_pause(snake_t* snake);
bool snake_text_update_game_over(snake_t* snake);
bool snake_text_update_resume_countdown(snake_t* snake, int seconds);
bool snake_text_update_titlebar(snake_t* snake);

#endif  // SNAKE_TEXT_H
