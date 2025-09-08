#ifndef SNAKE_H
#define SNAKE_H

#include "app.h"
#include "utils.h"

#define SNAKE_CELL_SIZE 10

#define SNAKE_GRID_X (APP_WINDOW_WIDTH / SNAKE_CELL_SIZE)
#define SNAKE_GRID_Y (APP_WINDOW_HEIGHT / SNAKE_CELL_SIZE)

// This is currently used to control the framerate, not the tickrate.
// TODO: Implement proper tickrate control separate from framerate.
#define SNAKE_TICK_RATE 10
#define SNAKE_TICK_INTERVAL (1000 / SNAKE_TICK_RATE)

typedef enum {
    SNAKE_DIRECTION_UP,
    SNAKE_DIRECTION_DOWN,
    SNAKE_DIRECTION_LEFT,
    SNAKE_DIRECTION_RIGHT
} snake_direction_t;

typedef enum {
    SNAKE_COLOR_BLACK,
    SNAKE_COLOR_GRAY,
    SNAKE_COLOR_GREEN,
    SNAKE_COLOR_RED
} snake_colors_t;

typedef struct {
    ivec2_t position;
    snake_colors_t color;
} snake_cell_t;

typedef struct {
    app_t app;

    snake_direction_t current_direction;

    ivec2_t position_head;
    ivec2_t previous_position_head;

    ivec2_t previous_position_tail;

    snake_cell_t cells[SNAKE_GRID_X][SNAKE_GRID_Y];

    dynamic_array_t array_food;
    dynamic_array_t array_body;

    TTF_Text* text_score;
    char text_score_buffer[32];
} snake_t;

bool snake_create(snake_t* snake, const char* title);
void snake_destroy(snake_t* snake);

void snake_handle_events(snake_t* snake);

void snake_update(snake_t* snake);
void snake_render(snake_t* snake);

#endif  // SNAKE_H
