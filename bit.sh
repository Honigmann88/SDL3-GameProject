#!/bin/bash
git clone https://github.com/libsdl-org/SDL.git vendored/SDL

cmake -S . -B build

cmake --build build

# ./build/hello
