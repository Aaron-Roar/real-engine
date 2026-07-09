CC = clang

PKGS = sdl3 sdl3-image ncurses
CFLAGS = -I. -Iexamples/test-assets $(shell pkg-config --cflags $(PKGS))
LIBS = $(shell pkg-config --libs $(PKGS)) -lm

ENGINE_SRC = \
	console.c \
	engine.c \
	entity_components.c \
	error.c \
	graphics.c \
	math2d.c \
	physics.c \
	systems.c \
	tools.c

ASSET_SRC = \
	examples/test-assets/elder-fly/elderfly_descriptors.c

all: build/examples/flies_in_pit build/examples/flies_around_ball

build/examples/flies_in_pit: examples/flies_in_pit/flies_in_pit.c $(ENGINE_SRC) $(ASSET_SRC)
	mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

build/examples/flies_around_ball: examples/flies_around_ball/flies_around_ball.c $(ENGINE_SRC) $(ASSET_SRC)
	mkdir -p build/examples
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

run-pit: build/examples/flies_in_pit
	./build/examples/flies_in_pit

run-ball: build/examples/flies_around_ball
	./build/examples/flies_around_ball

clean:
	rm -rf build
