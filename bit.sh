#!/bin/bash
git clone https://github.com/libsdl-org/SDL.git vendored/SDL
git clone https://github.com/libsdl-org/SDL_ttf.git vendored/SDL_ttf

cmake -S . -B build

cmake --build build

# ./build/hello
