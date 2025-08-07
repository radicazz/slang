#include <SDL3/SDL_main.h>

#include "snake.h"

int main(int argc, char* argv[]) {
    snake_t snake;
    if (snake_create(&snake, "Snake Game") == false) {
        return 1;
    }

    double frame_duration = 1000.0 / SNAKE_FPS;

    while (snake.is_running == true) {
        uint64_t frame_start = SDL_GetTicks();

        snake_handle_events(&snake);
        snake_render(&snake);

        uint64_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < frame_duration) {
            SDL_Delay(frame_duration - frame_time);
        }
    }

    snake_destroy(&snake);
    return 0;
}
