#include "graphics.h"
#include "console.h"
#include <stdio.h>
#include <SDL3/SDL.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Event event;

bool graphics_start() {
    console_write(LOG_ENGINE, "---Initializing Graphics---\n");
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        //Can fail need error
        return false;
    }

    console_write(LOG_ENGINE, "Starting game window and renderer\n");
    console_write(LOG_ENGINE, "Window width: %d\n", WINDOW_WIDTH);
    console_write(LOG_ENGINE, "Window height: %d\n", WINDOW_HEIGHT);
    if (!SDL_CreateWindowAndRenderer(
            "Game Test",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE,
            &window,
            &renderer
        )) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }

    console_write(LOG_ENGINE, "Configuring renderer\n");
    SDL_SetRenderLogicalPresentation(
        renderer,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_LOGICAL_PRESENTATION_LETTERBOX
    );

    console_write(LOG_ENGINE, "Graphics initialization complete\n");
    console_write(LOG_ENGINE, "---Initializing Graphics---\n");
    return true;
}

void renderer_end(SDL_Renderer *r) {
    SDL_DestroyRenderer(r);
    console_write(LOG_ENGINE, "Renderer terminated\n");
}

void window_end(SDL_Window *w) {
    SDL_DestroyWindow(w);
    console_write(LOG_ENGINE, "Window terminated\n");
}

void graphics_end() {
    console_write(LOG_ENGINE, "---Graphics Termination---\n");
    renderer_end(renderer);
    window_end(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    console_write(LOG_ENGINE, "SDL3 terminated\n");
    console_write(LOG_ENGINE, "Graphics termination complete\n");
    console_write(LOG_ENGINE, "---Graphics Termination---\n");

}

void graphics_poll_events() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            graphics_end();
        }
    }
}

void draw_background() {
    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, 33, 33, 33, SDL_ALPHA_OPAQUE);  /* dark gray, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */
}

void draw_rect(Shape rect, Position pos) {
    SDL_FRect sdl_rect;
    /* draw a filled rectangle in the middle of the canvas. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  /* blue, full alpha */
    sdl_rect.x = pos.x;
    sdl_rect.y = pos.y;
    sdl_rect.w = 20;
    sdl_rect.h = 20;
    SDL_RenderFillRect(renderer, &sdl_rect);
}

void show_graphics() {
    SDL_RenderPresent(renderer);
}
