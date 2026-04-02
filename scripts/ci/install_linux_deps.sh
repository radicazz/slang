#!/usr/bin/env bash

set -euo pipefail

sudo apt-get update
sudo apt-get install -y --no-install-recommends \
    cmake ninja-build pkg-config gcc g++ \
    libx11-dev libxext-dev libxrandr-dev libxrender-dev libxfixes-dev libxi-dev \
    libxkbcommon-dev libwayland-dev wayland-protocols \
    libegl1-mesa-dev libgl1-mesa-dev \
    libfreetype6-dev
