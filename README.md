# 2D Game Engine

A small, hand-built 2D game engine written in C.

This project is an experimental engine focused on understanding the systems underneath a game rather than hiding them behind a large framework. It includes a simple ECS-style entity/component architecture, SDL3 rendering, terminal logging/console support, 2D shape math, collision detection groundwork, and early rigid-body physics concepts.

The goal is not just to make objects move on screen. The goal is to build a clear, inspectable engine where physics, collision, rendering, and game state are separated enough to understand, test, and extend.

---

## Current Focus

The engine is currently being developed around a few core ideas:

* **Entity/component-style data storage**
* **2D rigid body motion**
* **Convex polygon geometry**
* **SAT collision detection**
* **SDL3 rendering**
* **Debug-friendly terminal console/logging**
* **Physics systems built from first principles**

The project is still early and intentionally low-level. Many systems are being built directly in C to make the math and engine architecture explicit.

---

## Why This Exists

Most game engines abstract away the interesting parts.

This project is my attempt to build the important systems myself:

* How entities store state
* How systems update motion
* How forces and torque affect bodies
* How local shapes become world-space shapes
* How convex polygons are projected onto axes
* How SAT determines overlap
* How rendering can be kept separate from physics
* How collision response can eventually conserve linear and angular momentum

It is part engine, part physics sandbox, and part learning project.

---

## Features

### ECS-Style Entity Storage

Entities are represented by integer IDs. Component data is stored in fixed-size arrays indexed by entity.

Current component-style data includes:

* Position
* Velocity
* Acceleration
* Mass
* Force
* Orientation
* Angular velocity
* Angular acceleration
* Torque
* Friction
* Restitution
* Hit boxes / collision shapes

Component masks are used to describe which components an entity owns.

---

### 2D Math and Geometry

The engine includes a growing `math2d` layer for common geometry and physics operations.

Current math/geometry support includes:

* 2D vectors
* Dot products
* Vector normalization
* Convex shape definitions
* Square creation
* Circle approximation using vertices
* Polygon centroid calculation
* Shape world transforms
* Projection of shapes onto axes
* Projection overlap calculation
* Edge normal generation
* SAT collision detection
* Polygon moment of inertia estimation

Shapes are stored as local-space vertices and transformed into world-space using position and orientation.

```text
local shape
    + position
    + orientation
        ↓
world-space shape
```

This keeps shape definitions clean while allowing entities to move and rotate independently.

---

### Collision Detection

Collision detection is currently based on the Separating Axis Theorem.

The core SAT flow is:

```text
1. Convert local hit boxes to world-space shapes.
2. Generate normals from each polygon edge.
3. Normalize those normals into test axes.
4. Project both shapes onto each axis.
5. Check for projection overlap.
6. If any axis has no overlap, there is no collision.
7. If all axes overlap, the shapes are colliding.
8. Track the smallest overlap as the collision depth.
```

The collision result stores:

```c
typedef struct Collision {
    bool overlap;
    Axis normal;
    Vec1D depth;
} Collision;
```

This gives enough information to start building collision resolution using a minimum translation vector:

```text
MTV = normal * depth
```

---

### Physics Systems

The current physics systems update linear and angular motion.

The update flow is roughly:

```text
clear acceleration
apply forces
apply torques
update velocity
update angular velocity
update orientation
update position
```

Linear motion follows:

```text
acceleration = force / mass
velocity += acceleration * dt
position += velocity * dt
```

Angular motion follows the same idea:

```text
angular_acceleration = torque / moment_of_inertia
angular_velocity += angular_acceleration * dt
orientation += angular_velocity * dt
```

The engine includes an early generic polygon moment of inertia calculation for uniformly dense convex shapes.

---

### SDL3 Rendering

Rendering is handled through SDL3.

Current rendering support includes:

* Window and renderer initialization
* Background clearing
* Shape outlines
* Filled convex shapes
* Hit box rendering
* RGB/RGBA color helpers
* Event polling

Convex shapes are rendered using their generic vertices. Filled polygons are drawn as triangle fans, while outlines are drawn by connecting the transformed vertices.

---

### Console and Logging

The engine includes a terminal-side console/logging layer.

This is used to keep runtime information visible while the SDL window is running. The goal is to eventually support an in-engine command/debug interface for inspecting state, controlling objects, or testing systems.

---

## Project Structure

```text
.
├── game.c                 # Main game loop and test setup
├── engine.c / engine.h    # Engine lifecycle
├── entity_components.c/h  # Entity IDs, component masks, component arrays
├── systems.c / systems.h  # Physics/update systems
├── math2d.c / math2d.h    # Geometry, shapes, SAT, inertia, vector math
├── graphics.c / graphics.h# SDL3 rendering and debug drawing
├── console.c / console.h  # Terminal console/logging
├── error.c / error.h      # Error reporting
├── tools.c / tools.h      # Utility functions
├── install.sh             # Simple build script
└── notes.md               # Development notes
```

---

## Building

The current build script uses `clang` and links against SDL3, math, and ncurses.

```bash
./install.sh
```

The script creates a build directory and outputs:

```text
./build/game.out
```

Then run:

```bash
./build/game.out
```

The current build expects the following dependencies to be available:

* `clang`
* `SDL3`
* `ncurses`
* standard C math library

On NixOS or inside a Nix shell, the intended dependency set is roughly:

```bash
nix-shell -p ncurses clang sdl3
```

Then:

```bash
./install.sh
./build/game.out
```

---

## Current Demo Behavior

The current `game.c` sets up test entities with shapes, velocity, mass, orientation, and torque. It updates physics each frame, checks collision between entities, and renders hit boxes differently depending on whether collision is detected.

At the moment, this is primarily a testbed for validating:

* shape transforms
* SAT overlap detection
* movement updates
* torque/angular motion
* SDL3 drawing
* debug rendering

---

## Roadmap

### Collision Response

The next major step is collision response.

Planned response work:

* Position correction using collision normal and depth
* Contact point generation
* Linear impulse response
* Angular impulse response
* Restitution / bounciness
* Friction impulse
* Static vs dynamic bodies
* Better manifold support

Target collision response model:

```text
normal impulse
    -> handles bounce / restitution

tangent impulse
    -> handles friction

r × J
    -> handles angular velocity changes
```

---

### Engine Architecture

Planned architecture improvements:

* Separate collision system file
* Separate render/debug draw system
* Cleaner material properties
* Store moment of inertia instead of recalculating every frame
* Improve entity deletion/reuse
* Improve error handling
* Add tests for math/physics functions
* Add a proper build system

---

### Gameplay Layer

Eventually, this engine is intended to support simple 2D gameplay experiments with:

* controllable entities
* collision-based interaction
* physics-driven movement
* debug commands
* generic renderable shapes
* game-specific systems layered above the engine core

---

## Design Philosophy

This engine is built around a few principles:

### Keep the math visible

The point is to understand the systems, not hide them. SAT projections, vector normals, torque, moment of inertia, and collision impulses are implemented directly.

### Keep data simple

Entity data is stored in arrays. Systems operate over entities that have the correct component masks.

### Keep rendering separate from physics

Shapes exist as local geometry. Physics transforms them into world-space. Rendering draws the result.

### Prefer working systems over perfect abstractions

The project is intentionally evolving. Some modules may be reorganized as the engine grows, but the current priority is building correct behavior step by step.

---

## Status

This project is a work in progress.

Currently working on:

* Convex shape rendering
* SAT collision detection
* Angular motion
* Collision data structures
* Preparing for physical collision response

Not yet complete:

* Full collision response
* Contact manifold generation
* Friction/restitution resolution
* Broad phase collision detection
* Asset loading
* Scene management
* Stable public API

---

## License

No license has been selected yet.

---

## Author

Built by Aaron Roar as a from-scratch 2D game engine and physics learning project.
