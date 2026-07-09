# Real Engine

A lightweight 2D game engine written in C using SDL3. The project focuses on building core engine systems from first principles rather than relying on existing game frameworks.

The long-term goal is to provide a modular engine for creating reactive 2D and 2.5D games while remaining easy to understand, extend, and experiment with.

---

## Current Features

* Entity Component System (ECS)
* Convex polygon collision detection (SAT)
* Rigid body physics
* Forces, torque, and joints
* Sprite rendering and animation
* Transform and axis constraints
* Terminal debugging interface (ncurses)
* Cross-platform build environment using Nix

---

## Project Structure

```text
engine/
├── console.*              # Terminal interface
├── engine.*               # Engine timing and lifecycle
├── entity_components.*    # ECS management
├── physics.*              # Physics data and utilities
├── systems.*              # Physics and gameplay systems
├── graphics.*             # Rendering
├── math2d.*               # Geometry and math
├── tools.*                # Utility functions
└── examples/              # Example games
```

---
## Building

### Dependencies
- C compiler: `clang` or `gcc`
- C standard library
- `make`
- `pkg-config`
- `sdl3`
- `sdl3-image`
- `ncurses`
- Math library: `libm` / `-lm`

Build the examples:

```bash
make
```

Run an example:

```bash
make run-pit
```

### NixOS / Nix

If you have Nix installed, enter the development environment:

```bash
nix develop
```
---

## Design Goals

* Simple, readable C code
* Modular engine architecture
* Data-oriented ECS
* Minimal dependencies
* Easily extensible systems
* Platform independent where practical

---

## Roadmap

* Input system
* Audio
* Camera system
* Scene serialization
* Particle system
* UI framework
* Networking
* Scripting support

---

This engine is an active personal project and continues to evolve as new engine systems are implemented and refined.

