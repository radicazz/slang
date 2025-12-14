#include <SDL3/SDL_main.h>

#include "snake.h"

int main(int argc, char* argv[]) {
    snake_t snake;
    if (snake_create(&snake, "snake") == false) {
        return 1;
    }

    while (snake.window.is_running == true) {
        snake_handle_events(&snake);
        while (snake.window.is_running == true &&
               window_can_update_fixed(&snake.window, WINDOW_TICK_INTERVAL) == true) {
            snake_update_fixed(&snake);
        }

        if (snake.window.is_running == false) {
            break;
        }

        snake_render_frame(&snake);
    }

    snake_destroy(&snake);
    return 0;
}
