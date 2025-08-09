#include <SDL3/SDL_main.h>

#include "snake.h"

int main(int argc, char* argv[]) {
    snake_t snake;
    if (snake_create(&snake, "snake") == false) {
        return 1;
    }

    while (snake.app.is_running == true) {
        if (app_should_tick(&snake.app, SNAKE_TICK_INTERVAL) == true) {
            snake_handle_events(&snake);
            snake_render(&snake);
        }
    }

    snake_destroy(&snake);
    return 0;
}
