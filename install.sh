#!/usr/bin/env bash
mkdir -p ./build
clang game.c entity_components.c systems.c tools.c error.c -o ./build/game.out
