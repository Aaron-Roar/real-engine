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
* Screen playback recording
* Terminal debugging interface (ncurses) "Implementation to be changed"

---

## Project Structure

```text
engine/
├── include/               # Public engine headers
├── src/                   # Engine implementation and private headers
├── docs/                  # Doxygen config and documentation pages
├── examples/              # Example games and assets
├── lib/                   # Generated static library output
└── build/                 # Generated objects, binaries, and docs
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
- `ffmpeg` "If using video recording"
- Math library: `libm` / `-lm`

Build the examples:

```bash
make build
```

Run an example:

```bash
make run-pit
```

Generate API documentation:

```bash
make docs
```

The generated HTML is written to:

```text
build/docs/html/index.html
```

Documentation source is committed in `docs/` and Doxygen comments in
`include/`. Generated HTML under `build/docs/` is not committed.

### NixOS / Nix

If you have Nix installed, enter the development environment:

```bash
nix develop
```

The Nix development shell includes the build tools, SDL dependencies, ffmpeg,
and Doxygen.
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
