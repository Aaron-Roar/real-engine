#!/usr/bin/env bash
#nix-shell -I nixpkgs=https://github.com/NixOS/nixpkgs/archive/nixos-unstable.tar.gz -p clang pkg-config sdl3 sdl3-image ncurses

mkdir -p ./build

clang \
  game.c \
  entity_components.c \
  systems.c \
  tools.c \
  error.c \
  graphics.c \
  console.c \
  engine.c \
  math2d.c \
  examples.c \
  test-assets/elder-fly/elderfly_descriptors.c \
  -I. \
  -Iinclude \
  -o ./build/game.out \
  $(pkg-config --cflags --libs sdl3 sdl3-image ncurses) \
  -lm

