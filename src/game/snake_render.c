#include "snake_render.h"

#include <SDL3/SDL_log.h>

#include "snake_menu.h"
#include "../modules/ui.h"

static const SDL_Color k_color_menu_overlay = {0, 0, 0, 160};
static const SDL_Color k_color_menu_panel = {25, 25, 25, 220};
static const SDL_Color k_color_menu_panel_border = {80, 80, 80, 255};
static const SDL_Color k_color_menu_button = {220, 220, 220, 255};
static const SDL_Color k_color_menu_button_border = {180, 180, 180, 255};

static bool get_text_size(snake_t* snake, TTF_Text* text, vector2i_t* out_size, const char* label) {
    SDL_assert(snake != NULL);
    SDL_assert(text != NULL);
    SDL_assert(out_size != NULL);
    SDL_assert(label != NULL);

    if (TTF_GetTextSize(text, &out_size->x, &out_size->y) == false) {
        SDL_Log("Failed to measure %s text: %s", label, SDL_GetError());
        snake->window.is_running = false;
        return false;
    }

    return true;
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

    if (snake->state == SNAKE_STATE_PAUSED || snake->state == SNAKE_STATE_START ||
        snake->state == SNAKE_STATE_GAME_OVER) {
        TTF_Text* title_text = NULL;
        TTF_Text* subtitle_text = NULL;
        bool has_subtitle = false;
        TTF_Text* button_text = NULL;

        if (snake->state == SNAKE_STATE_PAUSED) {
            title_text = snake->text_pause;
            button_text = snake->text_resume;
        } else if (snake->state == SNAKE_STATE_START) {
            title_text = snake->text_start_title;
            button_text = snake->text_start_button;
        } else {
            title_text = snake->text_game_over_title;
            subtitle_text = snake->text_game_over_score;
            has_subtitle = true;
            button_text = snake->text_restart_button;
        }

        snake_menu_layout_t layout;
        if (snake_menu_get_layout(snake, title_text, subtitle_text, has_subtitle, button_text, &layout) == false) {
            return;
        }

        if (SDL_SetRenderDrawBlendMode(snake->window.sdl_renderer, SDL_BLENDMODE_BLEND) == false) {
            SDL_Log("Failed to set blend mode: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        SDL_SetRenderDrawColor(snake->window.sdl_renderer, k_color_menu_overlay.r, k_color_menu_overlay.g,
                               k_color_menu_overlay.b, k_color_menu_overlay.a);

        SDL_FRect overlay_rect = {0.f, 0.f, (float)screen_size.x, (float)screen_size.y};
        SDL_RenderFillRect(snake->window.sdl_renderer, &overlay_rect);

        ui_panel_t panel;
        ui_panel_init(&panel, k_color_menu_panel, k_color_menu_panel_border);
        panel.rect = layout.panel_rect;
        if (ui_panel_render(snake->window.sdl_renderer, &panel) == false) {
            SDL_Log("Failed to render menu panel: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        ui_button_t button;
        ui_button_init(&button, k_color_menu_button, k_color_menu_button_border);
        button.rect = layout.button_rect;
        if (ui_button_render(snake->window.sdl_renderer, &button) == false) {
            SDL_Log("Failed to render menu button: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (TTF_DrawRendererText(title_text, layout.title_pos.x, layout.title_pos.y) == false) {
            SDL_Log("Failed to render menu title text: %s", SDL_GetError());
            snake->window.is_running = false;
            return;
        }

        if (layout.has_subtitle == true) {
            if (TTF_DrawRendererText(subtitle_text, layout.subtitle_pos.x, layout.subtitle_pos.y) == false) {
                SDL_Log("Failed to render menu subtitle text: %s", SDL_GetError());
                snake->window.is_running = false;
                return;
            }
        }

        vector2i_t button_text_size;
        if (get_text_size(snake, button_text, &button_text_size, "menu button") == false) {
            return;
        }

        float button_text_x = 0.f;
        float button_text_y = 0.f;
        ui_button_get_label_position(&button, &button_text_size, &button_text_x, &button_text_y);
        if (TTF_DrawRendererText(button_text, button_text_x, button_text_y) == false) {
            SDL_Log("Failed to render menu button text: %s", SDL_GetError());
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
