CC := clang
AR := ar

PKGS := sdl3 sdl3-image ncurses
CFLAGS := -Iinclude -Isrc -I. -Iexamples/test-assets $(shell pkg-config --cflags $(PKGS))
LIBS := $(shell pkg-config --libs $(PKGS)) -lm

ENGINE_SRC := \
	src/console.c \
	src/engine.c \
	src/error.c \
	src/entity_components.c \
	src/graphics.c \
	src/math2d.c \
	src/physics.c \
	src/systems.c \
	src/tools.c \
	src/level_editor.c\
	src/grid.c\
	src/controller.c

ENGINE_OBJ := $(patsubst src/%.c,build/obj/%.o,$(ENGINE_SRC))
ENGINE_LIB := lib/libreal_engine.a
DOXYGEN := doxygen
DOCS_DOXYFILE := docs/Doxyfile
DOCS_OUTPUT := build/docs

ASSET_SRC := \
	examples/test-assets/elder-fly/elderfly_descriptors.c\
	examples/test-assets/orm/orm_descriptors.c

PIT_BINARY := build/examples/flies_in_pit
BALL_BINARY := build/examples/flies_around_ball
VIEW_BINARY := build/examples/view_port

.PHONY: help all build build-engine build-example-pit build-example-ball run-pit run-ball docs clean-docs clean

help:
	@printf '%s\n' \
		"Build targets:" \
		"" \
		"  build" \
		"		  Builds all examples (flies_in_pit and flies_around_ball)" \
		"		  Example source code is located in the examples/ directory" \
		"		  Compiled binaries are output to build/examples/" \
		"" \
		"  all" \
		"		  Equivalent to make build" \
		"" \
		"  build-engine" \
		"		  Builds lib/libreal_engine.a" \
		"" \
		"  build-example-pit" \
		"		  Builds examples/flies-in-pit/flies_in_pit.c" \
		"		  Outputs build/examples/flies_in_pit" \
		"" \
		"  build-example-ball" \
		"		  Builds examples/flies-around-ball/flies_around_ball.c" \
		"		  Outputs build/examples/flies_around_ball" \
		"" \
		"  build-example-view" \
		"		  Builds examples/view-port/view_port.c" \
		"		  Outputs build/examples/view_port" \
		"" \
		"  run-pit" \
		"		  Builds and runs the flies_in_pit example" \
		"" \
		"  run-ball" \
		"		  Builds and runs the flies_around_ball example" \
		"" \
		"  run-view" \
		"		  Builds and runs the view_port example" \
		"" \
		"  docs" \
		"		  Builds Doxygen HTML docs into build/docs/html" \
		"" \
		"  clean-docs" \
		"		  Removes generated documentation" \
		"" \
		"  clean" \
		"		  Removes the build directory"

all: build

build: build-example-view build-example-pit build-example-ball

build-engine: $(ENGINE_LIB)

build-example-pit: $(PIT_BINARY)

build-example-ball: $(BALL_BINARY)

build-example-view: $(VIEW_BINARY)

$(ENGINE_LIB): $(ENGINE_OBJ)
	@mkdir -p lib
	$(AR) rcs $@ $^

build/obj/%.o: src/%.c
	@mkdir -p build/obj
	$(CC) -c $< $(CFLAGS) -o $@

$(PIT_BINARY): examples/flies-in-pit/flies_in_pit.c $(ENGINE_LIB) $(ASSET_SRC)
	@mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

$(BALL_BINARY): examples/flies-around-ball/flies_around_ball.c $(ENGINE_LIB) $(ASSET_SRC)
	@mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

$(VIEW_BINARY): examples/view-port/view_port.c $(ENGINE_LIB) $(ASSET_SRC)
	@mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

run-pit: $(PIT_BINARY)
	./$(PIT_BINARY)

run-ball: $(BALL_BINARY)
	./$(BALL_BINARY)

run-view: $(VIEW_BINARY)
	./$(VIEW_BINARY)

docs:
	$(DOXYGEN) $(DOCS_DOXYFILE)

clean-docs:
	rm -rf $(DOCS_OUTPUT)

clean:
	rm -rf build lib/*.a
