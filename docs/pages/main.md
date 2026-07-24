# Real Engine Documentation

Real Engine is a lightweight C and SDL3 game engine built around explicit
engine systems, stable entity ids, component tables, object pools, and a small
error/result API.

## Local Generation

From the project root:

```sh
nix develop
make docs
```

The generated HTML is written to:

```text
build/docs/html/index.html
```

Generated documentation is not committed. The committed documentation source is
the Doxygen comments in `include/`, this `docs/` directory, and the Doxygen
configuration.

## Useful Pages

- @ref architecture
- @ref entity_ids
- @ref errors
