# 🐍 Slang

[![GitHub license](https://img.shields.io/github/license/radicazz/slang.svg)](https://github.com/radicazz/slang/blob/master/LICENSE.txt)
[![GitHub last commit](https://img.shields.io/github/last-commit/radicazz/slang.svg)](https://github.com/radicazz/slang/commits/master)
[![C](https://img.shields.io/badge/C-99-blue.svg)](https://en.cppreference.com/w/c.html)
[![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)](https://cmake.org/)
[![SDL](https://img.shields.io/badge/SDL-3.0+-blue.svg)](https://www.libsdl.org/)

A modern implementation of the classic *snake* game made with C99 & SDL3 for fun.

## Gameplay

Navigate the snake with WASD or Arrow keys to eat apples, grow longer, and avoid collisions to achieve the highest score!

![Gameplay Screenshot](https://i.imgur.com/tTzlKck.gif)

## Requirements

> This project uses git submodules to manage dependencies. They may require additional dependencies themselves.

- [C Compiler](https://en.cppreference.com/w/c.html) (e.g., GCC, Clang, MSVC)
- [CMake](https://cmake.org/) (version 3.16 or higher)
- [SDL3](https://www.libsdl.org/) (automatically handled via submodules)

## Installation & Usage

1. Clone the repository with submodules:

    ```bash
    git clone --recurse-submodules https://github.com/radicazz/slang.git
    cd slang
    ```

2. Configure & build the project:

    ```bash
    cmake -S . -B build
    cmake --build build
    ```

3. Run the game:

    ```bash
    # On Linux/macOS
    ./build/slang
    ```

    ```bash
    # On Windows
    .\build\Debug\slang.exe
    ```

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE.txt](LICENSE.txt) file for details.
