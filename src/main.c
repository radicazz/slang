#include <SDL3/SDL_main.h>

#include "snake.h"

int main(int argc, char* argv[]) {
    snake_t snake;
    if (snake_create(&snake, "snake") == false) {
        return 1;
    }

    while (snake.app.is_running == true) {
        snake_handle_events(&snake);
        if (application_should_update_fixed(&snake.app, APP_TICK_INTERVAL) == true) {
            snake_update_fixed(&snake);
        }

        snake_render(&snake);
    }

    snake_destroy(&snake);
    return 0;
}
