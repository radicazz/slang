# 🐍 slang

[![tests](https://github.com/radicazz/slang/actions/workflows/tests.yml/badge.svg?branch=main)](https://github.com/radicazz/slang/actions/workflows/tests.yml)
[![License](https://img.shields.io/github/license/radicazz/slang.svg)](https://github.com/radicazz/slang/blob/main/LICENSE.txt)
[![Last Commit](https://img.shields.io/github/last-commit/radicazz/slang.svg)](https://github.com/radicazz/slang/commits/main)

My take on the classic snake game, built using C99, CMake and SDL3.

## Gameplay

[Here's an example of the game...](https://i.imgur.com/tTzlKck.gif)

- Use `arrows` or `WASD` to control the snake.
- Eat the red apples to grow longer and score points.
- Avoid running into yourself, or it's game over!
- Walls will wrap around to the other side of the screen.
- Use `esc` to pause and unpause the game.

## Requirements

> This project uses git submodules to manage dependencies. They may require additional dependencies themselves.

- [C Compiler](https://en.cppreference.com/w/c.html) (e.g., GCC, Clang, MSVC)
- [CMake](https://cmake.org/) (version 3.16 or higher)
- [SDL3](https://www.libsdl.org/) (automatically handled via submodules)

## Building

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
