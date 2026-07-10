CC := clang

PKGS := sdl3 sdl3-image ncurses
CFLAGS := -I. -Iexamples/test-assets $(shell pkg-config --cflags $(PKGS))
LIBS := $(shell pkg-config --libs $(PKGS)) -lm

ENGINE_SRC := \
	console.c \
	engine.c \
	entity_components.c \
	error.c \
	graphics.c \
	math2d.c \
	physics.c \
	systems.c \
	tools.c

ASSET_SRC := \
	examples/test-assets/elder-fly/elderfly_descriptors.c

PIT_BINARY := build/examples/flies_in_pit
BALL_BINARY := build/examples/flies_around_ball

.PHONY: help all build build-example-pit build-example-ball run-pit run-ball clean

help:
	@printf '%s\n' \
		"Build targets:" \
		"" \
		"  build" \
		"		  Builds all examples (flies_in_pit and flies_around_ball)." \
		"		  Example source code is located in the examples/ directory." \
		"		  Compiled binaries are output to build/examples/." \
		"" \
		"  all" \
		"		  Equivalent to make build." \
		"" \
		"  build-example-pit" \
		"		  Builds examples/flies_in_pit/flies_in_pit.c." \
		"		  Outputs build/examples/flies_in_pit." \
		"" \
		"  build-example-ball" \
		"		  Builds examples/flies_around_ball/flies_around_ball.c." \
		"		  Outputs build/examples/flies_around_ball." \
		"" \
		"  run-pit" \
		"		  Builds and runs the flies_in_pit example." \
		"" \
		"  run-ball" \
		"		  Builds and runs the flies_around_ball example." \
		"" \
		"  clean" \
		"		  Removes the build directory."

all: build

build: clean build-example-pit build-example-ball

build-example-pit: $(PIT_BINARY)

build-example-ball: $(BALL_BINARY)

$(PIT_BINARY): examples/flies_in_pit/flies_in_pit.c $(ENGINE_SRC) $(ASSET_SRC)
	@mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

$(BALL_BINARY): examples/flies_around_ball/flies_around_ball.c $(ENGINE_SRC) $(ASSET_SRC)
	@mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

run-pit: $(PIT_BINARY)
	./$(PIT_BINARY)

run-ball: $(BALL_BINARY)
	./$(BALL_BINARY)

clean:
	rm -rf build
