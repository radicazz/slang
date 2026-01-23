# 🐍 slang

[![Release](https://img.shields.io/github/v/release/radicazz/slang?include_prereleases&color=blue)](https://github.com/radicazz/slang/releases)
[![tests](https://github.com/radicazz/slang/actions/workflows/tests.yml/badge.svg?branch=main)](https://github.com/radicazz/slang/actions/workflows/tests.yml)
[![License](https://img.shields.io/github/license/radicazz/slang?color=yellow)](https://github.com/radicazz/slang/blob/main/LICENSE.txt)

My take on the classic snake game, built using C99, CMake and SDL3.

<!-- markdownlint-disable MD033 -->
<details>
<summary>Gameplay Example</summary>

![GIF of slang game showing a snake made of green segments on a dark background with a red apple](data/gameplay-example.gif)

</details>
<!-- markdownlint-enable MD033 -->

## Controls

- Use `arrows` or `wasd` to control the snake.
- Eat the red apples to grow longer and score points.
- Avoid running into yourself, or it's game over!
- Walls will wrap around to the other side of the screen.
- Use `esc` to pause and unpause the game.
- Use the Options button on the start or pause menus to adjust volume or mute.

## Config

The game writes a `config.ini` file next to the executable on all platforms. If the file is missing or invalid, it is
recreated with defaults.

Example:

```ini
high_score=12
mute=0
volume=0.80
resume_delay=2
```

## Building

> This project uses git submodules to manage dependencies. They may require additional dependencies themselves.

- [C Compiler](https://en.cppreference.com/w/c.html) (e.g., GCC, Clang, MSVC)
- [CMake](https://cmake.org/) (version 3.16 or higher)
- [SDL3](https://www.libsdl.org/) (automatically handled via submodules)

Start off by cloning the repository with its submodules:

```bash
# Clone the repository with submodules
git clone --recurse-submodules https://github.com/radicazz/slang.git
cd slang
```

Then, configure and build the project using CMake:

```bash
# Configure & build the project
cmake -S . -B build
cmake --build build
```

The compiled executable will be located in the `build` directory.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.
