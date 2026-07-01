#include "graphics.h"
#include "console.h"
#include <stdio.h>

const Color hit_box_color = (Color){255,0,0,255};

Color creat_color_hex(uint32_t hex) {
  return (Color) {
    .red = (hex >> 16) & 0xFF,
    .green = (hex >> 8)  & 0xFF,
    .blue = hex         & 0xFF,
    .alpha = 255
  };
}

Color create_color_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
  return (Color){
      .red = red,
      .blue = blue,
      .green = green,
      .alpha = alpha
  };
}

bool graphics_start(SDL_Renderer **renderer, SDL_Window **window) {
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
            window,
            renderer
        )) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }

    console_write(LOG_ENGINE, "Configuring renderer\n");
    SDL_SetRenderLogicalPresentation(
        *renderer,
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

void window_end(SDL_Window *window) {
    SDL_DestroyWindow(window);
    console_write(LOG_ENGINE, "Window terminated\n");
}

void graphics_end(SDL_Renderer *renderer, SDL_Window *window) {
    console_write(LOG_ENGINE, "---Graphics Termination---\n");
    renderer_end(renderer);
    window_end(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    console_write(LOG_ENGINE, "SDL3 terminated\n");
    console_write(LOG_ENGINE, "Graphics termination complete\n");
    console_write(LOG_ENGINE, "---Graphics Termination---\n");

}

bool graphics_poll_events(SDL_Event *event) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_EVENT_QUIT) {
            return false;
        }
    }
    return true;
}

void draw_background(SDL_Renderer *renderer, Color color) {
    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
    SDL_RenderClear(renderer);  /* start with a blank canvas. */
}

void draw_rect(SDL_Renderer *renderer, Shape rect, Position pos) {
    SDL_FRect sdl_rect;
    /* draw a filled rectangle in the middle of the canvas. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  /* blue, full alpha */
    sdl_rect.x = pos.x;
    sdl_rect.y = pos.y;
    sdl_rect.w = 20;
    sdl_rect.h = 20;
    SDL_RenderFillRect(renderer, &sdl_rect);
}

void show_graphics(SDL_Renderer *renderer) {
    SDL_RenderPresent(renderer);
}

bool draw_shape_outline(SDL_Renderer *renderer, Shape shape, Color color) {
    if (shape.amount_of_vertices < 2) {
        return false;
    }

    SDL_FPoint points[MAX_VERTICIES + 1];

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        points[i].x = shape.vertices[i].x;
        points[i].y = shape.vertices[i].y;
    }

    points[shape.amount_of_vertices] = points[0];

    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
    return SDL_RenderLines(renderer, points, shape.amount_of_vertices + 1);
}

bool draw_shape_filled(SDL_Renderer *renderer, Shape shape, Color color)
{
    if (shape.amount_of_vertices < 3) {
        return false;
    }

    SDL_Vertex vertices[MAX_VERTICIES];

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        vertices[i].position.x = shape.vertices[i].x;
        vertices[i].position.y = shape.vertices[i].y;

        vertices[i].color.r = color.red / 255.0f;
        vertices[i].color.g = color.green / 255.0f;
        vertices[i].color.b = color.blue / 255.0f;
        vertices[i].color.a = color.alpha / 255.0f;

        vertices[i].tex_coord.x = 0.0f;
        vertices[i].tex_coord.y = 0.0f;
    }

    int indices[(MAX_VERTICIES - 2) * 3];
    int index_count = 0;

    for (int i = 1; i < shape.amount_of_vertices - 1; i++) {
        indices[index_count++] = 0;
        indices[index_count++] = i;
        indices[index_count++] = i + 1;
    }

    return SDL_RenderGeometry(
        renderer,
        NULL,
        vertices,
        shape.amount_of_vertices,
        indices,
        index_count
    );
}

void draw_hit_box(SDL_Renderer *renderer, Entity e, Fill fill) {
    Shape shape = get_global_hit_box(e);
    if(fill == GRAPHICS_FILLED) {
        draw_shape_filled(renderer, shape, hit_box_color);
    }
    else {
        draw_shape_outline(renderer, shape, hit_box_color);
    }
}
