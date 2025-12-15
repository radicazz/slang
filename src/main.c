#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>

#include "snake.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    SDL_Log("Starting snake game");

    snake_t snake;
    if (snake_create(&snake, "snake") == false) {
        SDL_Log("Failed to initialize game, exiting");
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

    SDL_Log("Game shutting down");
    snake_destroy(&snake);
    return 0;
}
