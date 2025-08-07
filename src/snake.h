#ifndef SNAKE_H
#define SNAKE_H

#include <SDL3/SDL.h>

#include "utils.h"

#define SNAKE_WINDOW_HEIGHT 500
#define SNAKE_WINDOW_WIDTH 500
#define SNAKE_CELL_SIZE 10

// TODO: Find a better way to tick the game.
// This will suffice for now though.
#define SNAKE_FPS 30

#define SNAKE_WINDOW_X (SNAKE_WINDOW_WIDTH / SNAKE_CELL_SIZE)
#define SNAKE_WINDOW_Y (SNAKE_WINDOW_HEIGHT / SNAKE_CELL_SIZE)

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
    SDL_Window* window;
    SDL_Renderer* renderer;

    bool is_running;

    ivec2_t head_position;
    ivec2_t previous_head_position;
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
