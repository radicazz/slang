#!/usr/bin/env bash

set -euo pipefail

build_dir="${1:-build}"
shift || true

cmake -S . -B "${build_dir}" -G Ninja "$@"
cmake --build "${build_dir}"
ctest --test-dir "${build_dir}" --output-on-failure
