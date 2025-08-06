#include <SDL3/SDL_main.h>

#include "snake.h"

int main(int argc, char* argv[]) {
    snake_t snake;
    snake_init(&snake);

    if (snake_create(&snake, "Snake Game") == false) {
        return 1;
    }

    while (snake.is_running == true) {
        snake_handle_events(&snake);
        snake_render(&snake);
    }

    snake_destroy(&snake);
    return 0;
}
