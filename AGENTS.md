# slang: agent notes (not user docs)

## Quick commands

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Repo map

- `src/main.c`: main loop (fixed-timestep update + render).
- `src/snake.c`, `src/snake.h`: game init/teardown + public API.
- `src/game/snake_state.c`, `src/game/snake_state.h`: game state, movement, collisions, reset.
- `src/game/snake_input.c`, `src/game/snake_input.h`: input handling + menu actions.
- `src/game/snake_render.c`, `src/game/snake_render.h`: rendering (grid, score, menus).
- `src/game/snake_text.c`, `src/game/snake_text.h`: text creation/update/destroy.
- `src/game/snake_menu.c`, `src/game/snake_menu.h`: menu layout shared by input/render.
- `src/modules/window.c`, `src/modules/window.h`: SDL window/renderer + SDL_ttf init/teardown + timing.
- `src/modules/audio.c`, `src/modules/audio.h`: audio manager + sound loading/playing.
- `src/modules/ui.c`, `src/modules/ui.h`: reusable UI widgets (buttons/panels).
- `src/utils/*`: `vector2i_*` + `dynamic_array_*`.
- `tests/dynamic_array_tests.c`: unit test target (`dynamic_array_tests`).
- `tests/vector_tests.c`: unit test target (`vector_tests`).

## Conventions (follow these)

- C standard: C99 (`c_std_99`).
- Formatting: `clang-format -i` using `.clang-format` (4 spaces, 120 cols, left-aligned pointers).
- Error handling: check SDL/TTF return values (`false`/`NULL`), log via `SDL_Log(..., SDL_GetError())`, and fail fast.
- Lifetimes: `*_destroy()` must be safe after partial init; free resources once, then set pointers to `NULL`.
- Avoid unbounded loops; random placement must have an upper bound + a deterministic fallback.
- Prefer `snprintf` over `sprintf`; avoid implicit truncation.
- Commit messages: use Conventional Commit prefixes (`feat:`, `fix:`, `refactor:`, `chore:`, etc.) with a short summary.

## CMake notes

- Game sources are auto-globbed from `src/*.c`; adding new `.c` files under `src/` usually needs no CMake edit.
- Dependencies come from git submodules in `external/` (SDL3 + SDL_ttf).
- Post-build copies `assets/` into the executable directory.

## Don’ts

- Don’t commit build artifacts (`build/`, `CMakeCache.txt`, etc. are gitignored).
- Don’t add “clever” architecture; keep changes small and consistent with existing patterns.
