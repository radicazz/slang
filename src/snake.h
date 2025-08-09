#ifndef SNAKE_H
#define SNAKE_H

#include "app.h"
#include "utils.h"

#define SNAKE_WINDOW_HEIGHT 500
#define SNAKE_WINDOW_WIDTH 500

#define SNAKE_CELL_SIZE 20

#define SNAKE_WINDOW_X (SNAKE_WINDOW_WIDTH / SNAKE_CELL_SIZE)
#define SNAKE_WINDOW_Y (SNAKE_WINDOW_HEIGHT / SNAKE_CELL_SIZE)

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

    ivec2_t head_position;
    ivec2_t previous_head_position;
    ivec2_t previous_tail_position;
    snake_direction_t current_direction;

    snake_cell_t cells[SNAKE_WINDOW_X][SNAKE_WINDOW_Y];

    dynamic_array_t food;
    dynamic_array_t body;
} snake_t;

bool snake_create(snake_t* snake, const char* title);
void snake_destroy(snake_t* snake);

void snake_handle_events(snake_t* snake);
void snake_render(snake_t* snake);

#endif  // SNAKE_H
