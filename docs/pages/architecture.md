# Architecture {#architecture}

Real Engine keeps systems separate and explicit. Public engine APIs live in
`include/`, implementation files live in `src/`, and private engine contracts
that should not be application-facing live in `src/engine_internal.h`.

The engine initializes entity, physics, graphics, and grid tables during
`engine_init()`. Entity-indexed tables start with zero capacity and grow as
entities are added. `entity_add()` returns a stable entity id, while systems
resolve that id to the current component table index when table access is
needed.

The physics update pipeline is owned by `system_update_physics()` and currently
applies joints, forces, velocity/orientation integration, locks, global hitbox
updates, AABB/grid updates, and collision resolution.
