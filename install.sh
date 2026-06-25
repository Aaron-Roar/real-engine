#!/usr/bin/env bash
nix-shell -p ncurses clang sdl3
mkdir -p ./build
clang game.c entity_components.c systems.c tools.c error.c graphics.c console.c -o ./build/game.out -lSDL3 -lm -lncurses
