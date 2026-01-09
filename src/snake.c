#include "snake.h"
#include "modules/ui.h"

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SDL3/SDL_log.h>

static const SDL_Color k_color_empty = {0, 0, 0, 255};
static const SDL_Color k_color_food = {255, 0, 0, 255};
static const SDL_Color k_color_snake_head = {0, 255, 0, 255};
static const SDL_Color k_color_pause_overlay = {0, 0, 0, 160};
static const SDL_Color k_color_pause_panel = {25, 25, 25, 220};
static const SDL_Color k_color_pause_button = {220, 220, 220, 255};
static const SDL_Color k_color_pause_button_border = {180, 180, 180, 255};

static const float k_pause_panel_padding = 20.f;
static const float k_pause_text_gap = 16.f;
static const float k_pause_button_padding_x = 28.f;
static const float k_pause_button_padding_y = 12.f;

static bool update_pause_text(snake_t* snake);

static bool get_random_empty_position(snake_t* snake, vector2i_t* out_position) {
    SDL_assert(snake != NULL);
    SDL_assert(out_position != NULL);

    const int interior_width = SNAKE_GRID_X - 2;
    const int interior_height = SNAKE_GRID_Y - 2;
    const int max_attempts = interior_width * interior_height * 2;

    vector2i_t position;
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        vector2i_random(&position, interior_width, interior_height);
        snake_cell_t* cell = &snake->cells[position.x][position.y];
        if (cell->state == SNAKE_CELL_EMPTY) {
            *out_position = position;
            return true;
        }
    }

    for (int x = 1; x < SNAKE_GRID_X - 1; ++x) {
        for (int y = 1; y < SNAKE_GRID_Y - 1; ++y) {
            if (snake->cells[x][y].state == SNAKE_CELL_EMPTY) {
                vector2i_set(out_position, x, y);
                return true;
            }
        }
    }

    SDL_Log("Failed to find empty position on grid (grid full or no space available)");
    return false;
}

static void cell_set_state_and_color(snake_t* snake, const vector2i_t* position, snake_cell_state_t state,
                                     const SDL_Color* color) {
    SDL_assert(snake != NULL);
    SDL_assert(position != NULL);

    snake_cell_t* const cell = &snake->cells[position->x][position->y];
    cell->state = state;
    if (color != NULL) {
        cell->render_color = *color;
    }
}

static bool update_score_text(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_score != NULL);

    const int written =
        snprintf(snake->text_score_buffer, sizeof(snake->text_score_buffer), "Score: %zu", snake->array_body.size);
    if (written < 0 || (size_t)written >= sizeof(snake->text_score_buffer)) {
        SDL_Log("Failed to format score text.");
        return false;
    }

    if (TTF_SetTextString(snake->text_score, snake->text_score_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update score text: %s", SDL_GetError());
        return false;
    }

    return update_pause_text(snake);
}

static bool update_pause_text(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->text_pause != NULL);

    const int written =
        snprintf(snake->text_pause_buffer, sizeof(snake->text_pause_buffer), "Paused: %zu", snake->array_body.size);
    if (written < 0 || (size_t)written >= sizeof(snake->text_pause_buffer)) {
        SDL_Log("Failed to format pause text.");
        return false;
    }

    if (TTF_SetTextString(snake->text_pause, snake->text_pause_buffer, (size_t)written) == false) {
        SDL_Log("Failed to update pause text: %s", SDL_GetError());
        return false;
    }

    return true;
}

static void compute_pause_layout(const vector2i_t* screen_size, const vector2i_t* pause_text_size,
                                 const vector2i_t* resume_text_size, SDL_FRect* out_panel_rect,
                                 SDL_FRect* out_button_rect) {
    SDL_assert(screen_size != NULL);
    SDL_assert(pause_text_size != NULL);
    SDL_assert(resume_text_size != NULL);
    SDL_assert(out_panel_rect != NULL);
    SDL_assert(out_button_rect != NULL);

    ui_button_t button;
    ui_button_init(&button, k_color_pause_button, k_color_pause_button_border);
    ui_button_layout_from_label(&button, resume_text_size, 0.f, 0.f, k_pause_button_padding_x,
                                k_pause_button_padding_y);
    const float button_width = button.rect.w;
    const float button_height = button.rect.h;

    const float panel_width = SDL_max((float)pause_text_size->x, button_width) + k_pause_panel_padding * 2.f;
    const float panel_height =
        (float)pause_text_size->y + k_pause_text_gap + button_height + k_pause_panel_padding * 2.f;

    out_panel_rect->x = ((float)screen_size->x - panel_width) * 0.5f;
    out_panel_rect->y = ((float)screen_size->y - panel_height) * 0.5f;
    out_panel_rect->w = panel_width;
    out_panel_rect->h = panel_height;

    const float button_center_x = out_panel_rect->x + panel_width * 0.5f;
    const float button_center_y =
        out_panel_rect->y + k_pause_panel_padding + (float)pause_text_size->y + k_pause_text_gap + button_height * 0.5f;
    ui_button_layout_from_label(&button, resume_text_size, button_center_x, button_center_y, k_pause_button_padding_x,
                                k_pause_button_padding_y);
    *out_button_rect = button.rect;
}

static bool reset(snake_t* snake) {
    SDL_assert(snake != NULL);

    SDL_Log("Resetting game state");

    snake->is_paused = false;

    dynamic_array_destroy(&snake->array_food);
    dynamic_array_destroy(&snake->array_body);

    for (int x = 0; x < SNAKE_GRID_X; ++x) {
        for (int y = 0; y < SNAKE_GRID_Y; ++y) {
            snake_cell_t* cell = &snake->cells[x][y];

            cell->position.x = x;
            cell->position.y = y;

            cell_set_state_and_color(snake, &cell->position, SNAKE_CELL_EMPTY, &k_color_empty);
        }
    }

    vector2i_t head_position;
    if (get_random_empty_position(snake, &head_position) == false) {
        SDL_Log("Failed to find starting position for snake head");
        return false;
    }

    snake->position_head = head_position;
    cell_set_state_and_color(snake, &snake->position_head, SNAKE_CELL_SNAKE, &k_color_snake_head);
    snake->previous_position_head = snake->position_head;
    snake->previous_position_tail = snake->position_head;
    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_create(&snake->array_food, sizeof(vector2i_t), 8);
    for (int i = 0; i < snake->array_food.capacity; ++i) {
        vector2i_t food_position;
        if (get_random_empty_position(snake, &food_position) == false) {
            SDL_Log("Warning: Could only spawn %d food items", i);
            break;
        }

        dynamic_array_append(&snake->array_food, &food_position);
        cell_set_state_and_color(snake, &food_position, SNAKE_CELL_FOOD, &k_color_food);
    }

    dynamic_array_create(&snake->array_body, sizeof(vector2i_t), 8);

    if (update_score_text(snake) == false) {
        dynamic_array_destroy(&snake->array_food);
        dynamic_array_destroy(&snake->array_body);
        return false;
    }

    SDL_Log("Game reset complete (food items: %zu)", snake->array_food.size);
    return true;
}

bool snake_create(snake_t* snake, const char* title) {
    SDL_assert(snake != NULL);
    SDL_assert(title != NULL);

    SDL_Log("Initializing snake game");

    memset(snake, 0, sizeof(*snake));

    if (window_create(&snake->window, title, WINDOW_WIDTH, WINDOW_HEIGHT) == false) {
        SDL_Log("Failed to create game window");
        return false;
    }

    if (audio_manager_create(&snake->audio) == false) {
        SDL_Log("Warning: Failed to initialize audio, continuing without sound");
    } else {
        if (audio_manager_load_sound(&snake->audio, SOUND_EAT_FOOD, "assets/sounds/bubble-pop.wav") == false) {
            SDL_Log("Warning: Failed to load eating sound effect");
        }
    }

    Uint64 seed = SDL_GetTicksNS();
    seed ^= SDL_GetPerformanceCounter();
    seed ^= (Uint64)(uintptr_t)snake;
    if (seed == 0) {
        seed = SDL_GetPerformanceCounter() | 1u;
    }
    SDL_srand(seed);
    SDL_Log("RNG initialized with seed: %llu", (unsigned long long)seed);

    dynamic_array_init(&snake->array_food);
    dynamic_array_init(&snake->array_body);

    const int written = snprintf(snake->text_score_buffer, sizeof(snake->text_score_buffer), "Score: 0");
    if (written < 0 || (size_t)written >= sizeof(snake->text_score_buffer)) {
        SDL_Log("Failed to format initial score text");
        goto fail;
    }

    snake->text_score = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                       snake->text_score_buffer, (size_t)written);
    if (snake->text_score == NULL) {
        SDL_Log("Failed to create score text object: %s", SDL_GetError());
        goto fail;
    }

    if (TTF_SetTextColor(snake->text_score, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set score text color: %s", SDL_GetError());
        goto fail;
    }

    const int pause_written = snprintf(snake->text_pause_buffer, sizeof(snake->text_pause_buffer), "Paused: 0");
    if (pause_written < 0 || (size_t)pause_written >= sizeof(snake->text_pause_buffer)) {
        SDL_Log("Failed to format initial pause text");
        goto fail;
    }

    snake->text_pause = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default,
                                       snake->text_pause_buffer, (size_t)pause_written);
    if (snake->text_pause == NULL) {
        SDL_Log("Failed to create pause text object: %s", SDL_GetError());
        goto fail;
    }

    if (TTF_SetTextColor(snake->text_pause, 255, 255, 255, 255) == false) {
        SDL_Log("Failed to set pause text color: %s", SDL_GetError());
        goto fail;
    }

    const char* resume_label = "Resume";
    snake->text_resume = TTF_CreateText(snake->window.ttf_text_engine, snake->window.ttf_font_default, resume_label,
                                        SDL_strlen(resume_label));
    if (snake->text_resume == NULL) {
        SDL_Log("Failed to create resume text object: %s", SDL_GetError());
        goto fail;
    }

    if (TTF_SetTextColor(snake->text_resume, 20, 20, 20, 255) == false) {
        SDL_Log("Failed to set resume text color: %s", SDL_GetError());
        goto fail;
    }

    if (reset(snake) == false) {
        SDL_Log("Failed to initialize game state");
        goto fail;
    }

    SDL_Log("Snake game initialized successfully");
    return snake->window.is_running;

fail:
    snake_destroy(snake);
    return false;
}

void snake_destroy(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (snake->text_score != NULL) {
        TTF_DestroyText(snake->text_score);
        snake->text_score = NULL;
    }

    if (snake->text_pause != NULL) {
        TTF_DestroyText(snake->text_pause);
        snake->text_pause = NULL;
    }

    if (snake->text_resume != NULL) {
        TTF_DestroyText(snake->text_resume);
        snake->text_resume = NULL;
    }

    audio_manager_destroy(&snake->audio);
    window_destroy(&snake->window);

    vector2i_set(&snake->position_head, 0, 0);
    vector2i_set(&snake->previous_position_head, 0, 0);
    vector2i_set(&snake->previous_position_tail, 0, 0);

    snake->current_direction = SNAKE_DIRECTION_UP;

    dynamic_array_destroy(&snake->array_food);
    dynamic_array_destroy(&snake->array_body);
}

static void handle_movement_keys(snake_t* snake, SDL_Scancode scancode) {
    SDL_assert(snake != NULL);

    if (snake->is_paused == true) {
        return;
    }

    switch (scancode) {
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            if (snake->current_direction != SNAKE_DIRECTION_DOWN) {
                snake->current_direction = SNAKE_DIRECTION_UP;
            }
            break;

        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            if (snake->current_direction != SNAKE_DIRECTION_UP) {
                snake->current_direction = SNAKE_DIRECTION_DOWN;
            }
            break;

        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            if (snake->current_direction != SNAKE_DIRECTION_RIGHT) {
                snake->current_direction = SNAKE_DIRECTION_LEFT;
            }
            break;

        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            if (snake->current_direction != SNAKE_DIRECTION_LEFT) {
                snake->current_direction = SNAKE_DIRECTION_RIGHT;
            }
            break;
        default:
            break;
    }
}

static void move_head_and_body(snake_t* snake) {
    SDL_assert(snake != NULL);

    // Save the previous head position.
    snake->previous_position_head = snake->position_head;

    // Temporary head position to test collisions with border.
    vector2i_t new_head_position = snake->position_head;

    switch (snake->current_direction) {
        case SNAKE_DIRECTION_UP:
            new_head_position.y -= 1;
            break;
        case SNAKE_DIRECTION_DOWN:
            new_head_position.y += 1;
            break;
        case SNAKE_DIRECTION_LEFT:
            new_head_position.x -= 1;
            break;
        case SNAKE_DIRECTION_RIGHT:
            new_head_position.x += 1;
            break;
    }

    // Wrap around the screen edges.
    if (new_head_position.x == 0) {
        new_head_position.x = SNAKE_GRID_X - 2;
    }

    if (new_head_position.y == 0) {
        new_head_position.y = SNAKE_GRID_Y - 2;
    }

    if (new_head_position.x == SNAKE_GRID_X - 1) {
        new_head_position.x = 1;
    }

    if (new_head_position.y == SNAKE_GRID_Y - 1) {
        new_head_position.y = 1;
    }

    // Move the snake's head.
    snake->position_head = new_head_position;
    cell_set_state_and_color(snake, &snake->position_head, SNAKE_CELL_SNAKE, &k_color_snake_head);

    if (dynamic_array_is_empty(&snake->array_body) == true) {
        // Snake has no array_body, so clear the previous head position.
        cell_set_state_and_color(snake, &snake->previous_position_head, SNAKE_CELL_EMPTY, &k_color_empty);
    } else {
        vector2i_set(&snake->previous_position_tail, 0, 0);

        // Loop over the snake's array_body.
        for (size_t i = 0; i < snake->array_body.size; ++i) {
            vector2i_t* const current_body_position = (vector2i_t* const)dynamic_array_get(&snake->array_body, i);

            if (i == 0) {
                snake->previous_position_tail = *current_body_position;
                *current_body_position = snake->previous_position_head;
            } else {
                vector2i_t saved_position = snake->previous_position_tail;
                snake->previous_position_tail = *current_body_position;

                *current_body_position = saved_position;
            }

            cell_set_state_and_color(snake, current_body_position, SNAKE_CELL_SNAKE, NULL);
            cell_set_state_and_color(snake, &snake->previous_position_tail, SNAKE_CELL_EMPTY, &k_color_empty);
        }
    }
}

static void update_snake_gradient(snake_t* snake) {
    SDL_assert(snake != NULL);

    const Uint8 head_green = 255;
    const Uint8 tail_green = 120;
    const float knee = 0.3f;
    const float knee_weight = 0.7f;

    snake_cell_t* const head_cell = &snake->cells[snake->position_head.x][snake->position_head.y];
    head_cell->state = SNAKE_CELL_SNAKE;
    head_cell->render_color.r = 0;
    head_cell->render_color.g = head_green;
    head_cell->render_color.b = 0;
    head_cell->render_color.a = 255;

    if (snake->array_body.size == 0) {
        return;
    }

    const float start = (float)head_green;
    const float end = (float)tail_green;
    const float length = (float)snake->array_body.size;

    for (size_t i = 0; i < snake->array_body.size; ++i) {
        vector2i_t* const body_position = (vector2i_t* const)dynamic_array_get(&snake->array_body, i);
        snake_cell_t* const cell = &snake->cells[body_position->x][body_position->y];

        const float t = (float)(i + 1) / length;
        float eased_t;
        if (t <= knee) {
            eased_t = (t / knee) * knee_weight;
        } else {
            eased_t = knee_weight + ((t - knee) / (1.0f - knee)) * (1.0f - knee_weight);
        }

        float green_value = start + (end - start) * eased_t;
        if (green_value < 0.f) {
            green_value = 0.f;
        } else if (green_value > 255.f) {
            green_value = 255.f;
        }

        cell->state = SNAKE_CELL_SNAKE;
        cell->render_color.r = 0;
        cell->render_color.g = (Uint8)(green_value + 0.5f);
        cell->render_color.b = 0;
        cell->render_color.a = 255;
    }
}

bool test_body_collision(snake_t* snake) {
    SDL_assert(snake != NULL);

    for (size_t i = 0; i < snake->array_body.size; ++i) {
        const vector2i_t* const body_segment = (const vector2i_t* const)dynamic_array_get(&snake->array_body, i);
        if (vector2i_equals(&snake->position_head, body_segment) == true) {
            return true;
        }
    }

    return false;
}

static bool test_food_collision(snake_t* snake) {
    SDL_assert(snake != NULL);

    for (size_t i = 0; i < snake->array_food.size; ++i) {
        const vector2i_t* const food_position = (const vector2i_t* const)dynamic_array_get(&snake->array_food, i);

        // Food hit.
        if (vector2i_equals(&snake->position_head, food_position) == true) {
            dynamic_array_remove(&snake->array_food, i);

            vector2i_t new_food_position;
            if (get_random_empty_position(snake, &new_food_position) == true) {
                dynamic_array_append(&snake->array_food, &new_food_position);
                cell_set_state_and_color(snake, &new_food_position, SNAKE_CELL_FOOD, &k_color_food);
            }

            return true;
        }
    }

    return false;
}

void snake_update_fixed(snake_t* snake) {
    SDL_assert(snake != NULL);

    if (snake->is_paused == true) {
        return;
    }

    move_head_and_body(snake);

    if (test_body_collision(snake) == true) {
        SDL_Log("Collision detected! Score: %zu", snake->array_body.size);
        // Restart the game if the snake collides with its own array_body.
        if (reset(snake) == false) {
            SDL_Log("Failed to reset game after collision");
            snake->window.is_running = false;
        }
        return;
    }

    // Grow the snake if it hits array_food.
    if (test_food_collision(snake) == true) {
        audio_manager_play_sound(&snake->audio, SOUND_EAT_FOOD);

        vector2i_t new_segment_position;
        if (dynamic_array_is_empty(&snake->array_body) == true) {
            new_segment_position = snake->previous_position_head;
        } else {
            new_segment_position = snake->previous_position_tail;
        }

        dynamic_array_append(&snake->array_body, &new_segment_position);

        if (update_score_text(snake) == false) {
            snake->window.is_running = false;
        }
    }

    update_snake_gradient(snake);
}

void snake_handle_events(snake_t* snake) {
    SDL_assert(snake != NULL);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            snake->window.is_running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDL_SCANCODE_ESCAPE && event.key.repeat == 0) {
                snake->is_paused = !snake->is_paused;
                if (update_pause_text(snake) == false) {
                    snake->window.is_running = false;
                }
            }

            handle_movement_keys(snake, event.key.scancode);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && snake->is_paused == true &&
            event.button.button == SDL_BUTTON_LEFT && event.button.down == true) {
            vector2i_t screen_size;
            if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &screen_size.x, &screen_size.y) == false) {
                SDL_Log("Failed to query render output size: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }

            vector2i_t pause_text_size;
            if (TTF_GetTextSize(snake->text_pause, &pause_text_size.x, &pause_text_size.y) == false) {
                SDL_Log("Failed to measure pause text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }

            vector2i_t resume_text_size;
            if (TTF_GetTextSize(snake->text_resume, &resume_text_size.x, &resume_text_size.y) == false) {
                SDL_Log("Failed to measure resume text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }

            SDL_FRect panel_rect;
            SDL_FRect button_rect;
            compute_pause_layout(&screen_size, &pause_text_size, &resume_text_size, &panel_rect, &button_rect);

            ui_button_t button;
            ui_button_init(&button, k_color_pause_button, k_color_pause_button_border);
            button.rect = button_rect;

            if (ui_button_contains(&button, event.button.x, event.button.y) == true) {
                snake->is_paused = false;
            }
        }
    }
}

void snake_render_frame(snake_t* snake) {
    SDL_assert(snake != NULL);
    SDL_assert(snake->window.sdl_window != NULL);
    SDL_assert(snake->window.sdl_renderer != NULL);

    if (SDL_RenderClear(snake->window.sdl_renderer) == false) {
        SDL_Log("Failed to clear renderer: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    // Loop through all the cells and render them based on their color.
    for (int x = 0; x < SNAKE_GRID_X; ++x) {
        for (int y = 0; y < SNAKE_GRID_Y; ++y) {
            const snake_cell_t* const cell = &snake->cells[x][y];

            // TODO: Optimize the use of SDL_SetRenderDrawColor by rendering all tiles of the same color at once.
            SDL_SetRenderDrawColor(snake->window.sdl_renderer, cell->render_color.r, cell->render_color.g,
                                   cell->render_color.b, cell->render_color.a);

            SDL_FRect rect;
            rect.x = (float)(cell->position.x * SNAKE_CELL_SIZE);
            rect.y = (float)(cell->position.y * SNAKE_CELL_SIZE);
            rect.w = rect.h = (float)SNAKE_CELL_SIZE;

            SDL_RenderFillRect(snake->window.sdl_renderer, &rect);
        }
    }

    vector2i_t screen_size;
    if (SDL_GetCurrentRenderOutputSize(snake->window.sdl_renderer, &screen_size.x, &screen_size.y) == false) {
        SDL_Log("Failed to query render output size: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    vector2i_t text_size;
    if (TTF_GetTextSize(snake->text_score, &text_size.x, &text_size.y) == false) {
        SDL_Log("Failed to measure score text: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    // Render the score text at the top center of the screen.
    if (TTF_DrawRendererText(snake->text_score, (float)(screen_size.x - text_size.x) * 0.5f, 10.f) == false) {
        SDL_Log("Failed to render score text: %s", SDL_GetError());
        snake->window.is_running = false;
        return;
    }

    if (snake->is_paused == true) {
        vector2i_t pause_text_size;
        if (TTF_GetTextSize(snake->text_pause, &pause_text_size.x, &pause_text_size.y) == false) {
            SDL_Log("Failed to measure pause text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        vector2i_t resume_text_size;
        if (TTF_GetTextSize(snake->text_resume, &resume_text_size.x, &resume_text_size.y) == false) {
            SDL_Log("Failed to measure resume text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        SDL_FRect panel_rect;
        SDL_FRect button_rect;
        compute_pause_layout(&screen_size, &pause_text_size, &resume_text_size, &panel_rect, &button_rect);

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_BLEND) == false) {
            SDL_Log("Failed to set blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_pause_overlay.r, k_color_pause_overlay.g,
                               k_color_pause_overlay.b, k_color_pause_overlay.a);

        SDL_FRect overlay_rect = {0.f, 0.f, (float)screen_size.x, (float)screen_size.y};
        SDL_RenderFillRect(snake->window.sdl_renderer, &overlay_rect);

        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_pause_panel.r, k_color_pause_panel.g,
                               k_color_pause_panel.b, k_color_pause_panel.a);
        SDL_RenderFillRect(snake->window.sdl_renderer, &panel_rect);

        ui_button_t button;
        ui_button_init(&button, k_color_pause_button, k_color_pause_button_border);
        button.rect = button_rect;
        if (ui_button_render(snake->window.sdl_renderer, &button) == false) {
            SDL_Log("Failed to render pause button: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        const float pause_text_x = panel_rect.x + (panel_rect.w - (float)pause_text_size.x) * 0.5f;
        const float pause_text_y = panel_rect.y + k_pause_panel_padding;
        if (TTF_DrawRendererText(snake->text_pause, pause_text_x, pause_text_y) == false) {
            SDL_Log("Failed to render pause text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        float resume_text_x = 0.f;
        float resume_text_y = 0.f;
        ui_button_get_label_position(&button, &resume_text_size, &resume_text_x, &resume_text_y);
        if (TTF_DrawRendererText(snake->text_resume, resume_text_x, resume_text_y) == false) {
            SDL_Log("Failed to render resume text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_NONE) == false) {
            SDL_Log("Failed to reset blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }
    }

    SDL_RenderPresent(snake->window.sdl_renderer);
}
