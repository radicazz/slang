#ifndef SNAKE_H
#define SNAKE_H

#include "modules/window.h"
#include "modules/audio.h"
#include "utils/vector.h"
#include "utils/dynamic_array.h"

#define SNAKE_CELL_SIZE 10

#define SNAKE_GRID_X (WINDOW_WIDTH / SNAKE_CELL_SIZE)
#define SNAKE_GRID_Y (WINDOW_HEIGHT / SNAKE_CELL_SIZE)

typedef enum {
    SNAKE_DIRECTION_UP,
    SNAKE_DIRECTION_DOWN,
    SNAKE_DIRECTION_LEFT,
    SNAKE_DIRECTION_RIGHT
} snake_direction_t;

typedef enum { SNAKE_CELL_EMPTY, SNAKE_CELL_WALL, SNAKE_CELL_FOOD, SNAKE_CELL_SNAKE } snake_cell_state_t;

typedef struct {
    vector2i_t position;
    snake_cell_state_t state;
    SDL_Color render_color;
} snake_cell_t;

typedef struct {
    window_t window;
    audio_manager_t audio;

    bool is_paused;

    snake_direction_t current_direction;

    vector2i_t position_head;
    vector2i_t previous_position_head;
    vector2i_t previous_position_tail;

    snake_cell_t cells[SNAKE_GRID_X][SNAKE_GRID_Y];

    dynamic_array_t array_food;
    dynamic_array_t array_body;

    TTF_Text* text_score;
    char text_score_buffer[32];

    TTF_Text* text_pause;
    char text_pause_buffer[32];

    TTF_Text* text_resume;
} snake_t;

bool snake_create(snake_t* snake, const char* title);
void snake_destroy(snake_t* snake);

void snake_handle_events(snake_t* snake);
void snake_update_fixed(snake_t* snake);
void snake_render_frame(snake_t* snake);

#endif  // SNAKE_H
