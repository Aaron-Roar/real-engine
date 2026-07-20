#include "graphics.h"
#include "console.h"
#include "engine.h"
#include "systems.h"
#include "physics.h"
#include <stdio.h>
#include "grid.h"

AnimatedSprite animated_sprites[MAX_ENTITIES] = {0};
const Color hit_box_color = (Color){255,0,0,255};
const Color particle_color = (Color){0,0,255,255};

#include <stdint.h>
#include <string.h>

typedef struct ScreenRecorder {
    FILE *ffmpeg_pipe;

    bool recording;
    int fps;
    int width;
    int height;

    char output_path[512];
} ScreenRecorder;

static ScreenRecorder screen_recorder = {0};

bool graphics_recording_start(const char *output_path, int fps) {
    if(screen_recorder.recording) {
        return false;
    }

    if(output_path == NULL || fps <= 0) {
        return false;
    }

    screen_recorder = (ScreenRecorder){
        .ffmpeg_pipe = NULL,
        .recording = true,
        .fps = fps,
        .width = 0,
        .height = 0
    };

    snprintf(
        screen_recorder.output_path,
        sizeof(screen_recorder.output_path),
        "%s",
        output_path
    );

    return true;
}
static bool graphics_recording_open_ffmpeg(int width, int height) {
    char command[1024];

    snprintf(
        command,
        sizeof(command),

        "ffmpeg -y "
        "-loglevel warning "
        "-f rawvideo "
        "-pixel_format rgba "
        "-video_size %dx%d "
        "-framerate %d "
        "-i pipe:0 "
        "-an "
        "-c:v libx264 "
        "-preset ultrafast "
        "-crf 30 "
        "-pix_fmt yuv420p "
        "\"%s\"",

        width,
        height,
        screen_recorder.fps,
        screen_recorder.output_path
    );

    screen_recorder.ffmpeg_pipe =
        popen(command, "w");

    if(screen_recorder.ffmpeg_pipe == NULL) {
        console_write(
            LOG_ENGINE,
            "Failed to start FFmpeg recording\n"
        );

        return false;
    }

    screen_recorder.width = width;
    screen_recorder.height = height;

    console_write(
        LOG_ENGINE,
        "Started recording %dx%d at %d FPS\n",
        width,
        height,
        screen_recorder.fps
    );

    return true;
}

static bool graphics_record_frame(SDL_Renderer *renderer) {
    if(!screen_recorder.recording) {
        return true;
    }

    SDL_FRect presentation_frect;

    if(!SDL_GetRenderLogicalPresentationRect(
        renderer,
        &presentation_frect
    )) {
        console_write(
            LOG_ENGINE,
            "Failed to get presentation rect: %s\n",
            SDL_GetError()
        );

        return false;
    }

    SDL_Rect presentation_rect = {
        .x = (int)presentation_frect.x,
        .y = (int)presentation_frect.y,
        .w = (int)presentation_frect.w,
        .h = (int)presentation_frect.h
    };

    if(presentation_rect.w <= 0 ||
       presentation_rect.h <= 0) {
        return true;
    }

    SDL_Surface *captured =
        SDL_RenderReadPixels(
            renderer,
            &presentation_rect
        );

    if(captured == NULL) {
        console_write(
            LOG_ENGINE,
            "Failed to capture frame: %s\n",
            SDL_GetError()
        );

        return false;
    }

    SDL_Surface *scaled =
        SDL_ScaleSurface(
            captured,
            RECORDING_WIDTH,
            RECORDING_HEIGHT,
            SDL_SCALEMODE_LINEAR
        );

    SDL_DestroySurface(captured);

    if(scaled == NULL) {
        console_write(
            LOG_ENGINE,
            "Failed to scale recorded frame: %s\n",
            SDL_GetError()
        );

        return false;
    }

    SDL_Surface *rgba_surface =
        SDL_ConvertSurface(
            scaled,
            SDL_PIXELFORMAT_RGBA32
        );

    SDL_DestroySurface(scaled);

    if(rgba_surface == NULL) {
        console_write(
            LOG_ENGINE,
            "Failed to convert recorded frame: %s\n",
            SDL_GetError()
        );

        return false;
    }

    if(screen_recorder.ffmpeg_pipe == NULL) {
        if(!graphics_recording_open_ffmpeg(
            RECORDING_WIDTH,
            RECORDING_HEIGHT
        )) {
            SDL_DestroySurface(rgba_surface);
            return false;
        }
    }

    size_t bytes_per_row =
        (size_t)RECORDING_WIDTH * 4;

    uint8_t *pixels =
        (uint8_t *)rgba_surface->pixels;

    for(int y = 0; y < RECORDING_HEIGHT; y++) {
        uint8_t *row =
            pixels + y * rgba_surface->pitch;

        if(fwrite(
            row,
            1,
            bytes_per_row,
            screen_recorder.ffmpeg_pipe
        ) != bytes_per_row) {
            console_write(
                LOG_ENGINE,
                "Failed writing recorded frame\n"
            );

            SDL_DestroySurface(rgba_surface);
            return false;
        }
    }

    SDL_DestroySurface(rgba_surface);
    return true;
}
void graphics_recording_stop(void) {
    if(!screen_recorder.recording) {
        return;
    }

    if(screen_recorder.ffmpeg_pipe != NULL) {
        int result =
            pclose(screen_recorder.ffmpeg_pipe);

        if(result != 0) {
            console_write(
                LOG_ENGINE,
                "FFmpeg exited with status: %d\n",
                result
            );
        }
    }

    screen_recorder = (ScreenRecorder){0};

    console_write(
        LOG_ENGINE,
        "Recording stopped\n"
    );
}

void graphics_draw_grid(SDL_Renderer *renderer) {
    Color grid_color = {100, 100, 100, 255};

    SDL_SetRenderDrawColor(
        renderer,
        grid_color.red,
        grid_color.green,
        grid_color.blue,
        grid_color.alpha
    );

    float grid_min_x =
        -(GRID_COLS * CELL_SIZE) * 0.5f;

    float grid_max_x =
        (GRID_COLS * CELL_SIZE) * 0.5f;

    float grid_min_y =
        -(GRID_ROWS * CELL_SIZE) * 0.5f;

    float grid_max_y =
        (GRID_ROWS * CELL_SIZE) * 0.5f;

    for(int col = 0; col <= GRID_COLS; col++) {
        float world_x =
            grid_min_x + col * CELL_SIZE;

        if(world_x < -WINDOW_WIDTH * 0.5f ||
           world_x >  WINDOW_WIDTH * 0.5f) {
            continue;
        }

        Position top = graphics_world_to_screen(
            (Position){
                .x = world_x,
                .y = grid_max_y
            }
        );

        Position bottom = graphics_world_to_screen(
            (Position){
                .x = world_x,
                .y = grid_min_y
            }
        );

        SDL_FPoint points[2] = {
            {top.x, top.y},
            {bottom.x, bottom.y}
        };

        SDL_RenderLines(renderer, points, 2);
    }
    for(int row = 0; row <= GRID_ROWS; row++) {
        float world_y =
            grid_min_y + row * CELL_SIZE;

        if(world_y < -WINDOW_HEIGHT * 0.5f ||
           world_y >  WINDOW_HEIGHT * 0.5f) {
            continue;
        }

        Position left = graphics_world_to_screen(
            (Position){
                .x = grid_min_x,
                .y = world_y
            }
        );

        Position right = graphics_world_to_screen(
            (Position){
                .x = grid_max_x,
                .y = world_y
            }
        );

        SDL_FPoint points[2] = {
            {left.x, left.y},
            {right.x, right.y}
        };

        SDL_RenderLines(renderer, points, 2);
    }
}

void graphics_scale_textures(Entity entity, Scale scale) {
    for(int i = 0; i < MAX_TEXTURES; i += 1) {
        animated_sprites[entity].animation.texture_list.textures[i].size.x *= scale.x;
        animated_sprites[entity].animation.texture_list.textures[i].size.y *= scale.y;
    }
}

Color graphics_creat_color_hex(uint32_t hex_color_code) {
  return (Color) {
    .red = (hex_color_code >> 16) & 0xFF,
    .green = (hex_color_code >> 8)  & 0xFF,
    .blue = hex_color_code         & 0xFF,
    .alpha = 255
  };
}

Color graphics_create_color_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
  return (Color){
      .red = red,
      .blue = blue,
      .green = green,
      .alpha = alpha
  };
}

Position graphics_world_to_screen(Position world) {
    return (Position){
        .x = world.x + WINDOW_WIDTH * 0.5f,
        .y = WINDOW_HEIGHT * 0.5f - world.y
    };
}

Position graphics_screen_to_world(Position screen) {
    return (Position){
        .x = screen.x - WINDOW_WIDTH * 0.5f,
        .y = WINDOW_HEIGHT * 0.5f - screen.y
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

void graphics_renderer_end(SDL_Renderer *r) {
    SDL_DestroyRenderer(r);
    console_write(LOG_ENGINE, "Renderer terminated\n");
}

void graphics_window_end(SDL_Window *window) {
    SDL_DestroyWindow(window);
    console_write(LOG_ENGINE, "Window terminated\n");
}

void graphics_end(SDL_Renderer *renderer, SDL_Window *window) {
    console_write(LOG_ENGINE, "---Graphics Termination---\n");
    graphics_recording_stop();
    graphics_renderer_end(renderer);
    graphics_window_end(window);
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

void graphics_draw_background(SDL_Renderer *renderer, Color color) {
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
    SDL_RenderClear(renderer);
}

void graphics_draw_rect(SDL_Renderer *renderer, Shape rect, Position pos) {
    SDL_FRect sdl_rect;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  /* blue, full alpha */
    Position screen_loc = graphics_world_to_screen(pos);
    sdl_rect.x = screen_loc.x;
    sdl_rect.y = screen_loc.y;
    sdl_rect.w = 20;
    sdl_rect.h = 20;
    SDL_RenderFillRect(renderer, &sdl_rect);
}

void graphics_show(SDL_Renderer *renderer) {
      if(screen_recorder.recording) {
        if(!graphics_record_frame(renderer)) {
            graphics_recording_stop();
        }
    }
    SDL_RenderPresent(renderer);
}

bool graphics_draw_shape_outline(SDL_Renderer *renderer, Shape shape, Color color) {
    if (shape.amount_of_vertices < 2) {
        return false;
    }

    SDL_FPoint points[MAX_VERTICIES + 1];

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        Position screen_loc = graphics_world_to_screen(shape.vertices[i]);
        points[i].x = screen_loc.x;
        points[i].y = screen_loc.y;
    }

    points[shape.amount_of_vertices] = points[0];

    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
    return SDL_RenderLines(renderer, points, shape.amount_of_vertices + 1);
}

bool graphics_draw_shape_filled(SDL_Renderer *renderer, Shape shape, Color color) {
    if (shape.amount_of_vertices < 3) {
        return false;
    }

    SDL_Vertex vertices[MAX_VERTICIES];

    for (int i = 0; i < shape.amount_of_vertices; i++) {
        Position screen_loc = graphics_world_to_screen(shape.vertices[i]);
        vertices[i].position.x = screen_loc.x;
        vertices[i].position.y = screen_loc.y;

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

void graphics_draw_hit_box(SDL_Renderer *renderer, Entity entity, Fill fill_type) {
    Shape shape = physics_get_global_hit_box(entity);
    if(fill_type == GRAPHICS_FILLED) {
        graphics_draw_shape_filled(renderer, shape, hit_box_color);
    }
    else {
        graphics_draw_shape_outline(renderer, shape, hit_box_color);
    }
}

void graphics_draw_hit_boxes(SDL_Renderer *renderer) {
  for(int i = 0; i < MAX_ENTITIES; i += 1) {
    if(entity_alive[i]) {
        if( (entity_mask[i] & HIT_BOX) == HIT_BOX) {
            graphics_draw_hit_box(renderer, i, GRAPHICS_OUTLINE);
        }
    }
  }
}

void graphics_draw_particle(SDL_Renderer *renderer, Entity entity, Fill fill_type) {
    Shape shape = physics_get_global_hit_box(entity);
    float radius = math_circle_radius(shape,math_polygon_centroid(shape));
    Shape circle = math_create_circle(radius, 10);
    Shape world_circle = physics_shape_world_translate(circle, positions[entity], 0);
    if(fill_type == GRAPHICS_FILLED) {
        graphics_draw_shape_filled(renderer, world_circle, particle_color);
    }
    else {
        graphics_draw_shape_outline(renderer, world_circle, particle_color);
    }
}
void graphics_draw_particles(SDL_Renderer *renderer) {
  for(int i = 0; i < MAX_ENTITIES; i += 1) {
    if(entity_alive[i]) {
        if( (entity_mask[i] & HIT_BOX) == HIT_BOX) {
          if( (entity_mask[i] & PARTICLE) == PARTICLE) {
              graphics_draw_particle(renderer, i, GRAPHICS_OUTLINE);
          }
        }
    }
  }
}
TextureAsset graphics_load_texture(SDL_Renderer *renderer, TextureDescriptor text_desc) {
        SDL_Surface *surface = NULL;
        char *png_path = NULL;
        TextureAsset asset = {0};
        asset.size = (Scale){
            .x = text_desc.size.x,
            .y = text_desc.size.y,
        };

        SDL_asprintf(&png_path, "%s", text_desc.file);
        surface = SDL_LoadPNG(png_path);
        SDL_free(png_path);

        asset.texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        return asset;
}

AnimationAsset graphics_load_animation(SDL_Renderer *renderer, AnimationDescriptor anim_desc) {
    AnimationAsset asset = {0};
    asset.texture_list.amount = anim_desc.amount_of_descriptors;
    asset.ticks_per_frame = anim_desc.ticks_per_frame;
    asset.time_per_frame = anim_desc.time_per_frame;

    for(int i = 0; i < anim_desc.amount_of_descriptors; i += 1) {
        TextureAsset texture = graphics_load_texture(renderer, anim_desc.texture_descriptors[i]);
        asset.texture_list.textures[i] = texture;
    }

    return asset;
}

AnimatedSprite graphics_create_animated_sprite(AnimationAsset asset_ptr, Scale scale) {
    AnimatedSprite sprite = {0};
    sprite.animation = asset_ptr;
    sprite.animation_frame = 0;
    sprite.direction = DIRECTION_RIGHT;
    sprite.scale = scale;
    sprite.last_update_tick = 0;
    sprite.last_update_time = 0;

    return sprite;
}

void graphics_update_sprite_frame(AnimatedSprite *sprite, Tick current_tick, Time current_time) {
    bool frame_need_update_tick = (sprite->animation.ticks_per_frame <= (current_tick - sprite->last_update_tick)) && (sprite->animation.ticks_per_frame != 0);
    bool frame_need_update_time = (sprite->animation.time_per_frame <= (current_time - sprite->last_update_time) && (sprite->animation.time_per_frame != 0));

    if(frame_need_update_tick || frame_need_update_time) {
        sprite->animation_frame = (sprite->animation_frame + 1)%sprite->animation.texture_list.amount;
        sprite->last_update_tick = current_tick;
        sprite->last_update_time = current_time;
    }
}

void graphics_draw_texture(SDL_Renderer *renderer, TextureAsset texture_asset, Position pos, Orientation ort) {
    SDL_FRect dst_rect = {0};
    Position screen_loc = graphics_world_to_screen(pos);
    dst_rect.w = texture_asset.size.x;//(float) texture_width;
    dst_rect.h = texture_asset.size.y;//(float) texture_width;
    dst_rect.x = screen_loc.x - dst_rect.w * 0.5f;//(float) texture_width;
    dst_rect.y = screen_loc.y - dst_rect.h * 0.5f;//(float) texture_height;

    SDL_FPoint center = {
        .x = dst_rect.w * 0.5f,
        .y = dst_rect.h * 0.5f
    };
    double degrees = -(double)ort * 180.0 / (double)PI_F;
    SDL_RenderTextureRotated(
        renderer,
    texture_asset.texture,
    NULL,
    &dst_rect,
    degrees,
    &center,
    SDL_FLIP_NONE
    );
}

void graphics_draw_sprite(SDL_Renderer *renderer, AnimatedSprite sprite, Position pos, Orientation ort) {
    TextureAsset asset = {0};
    asset = sprite.animation.texture_list.textures[sprite.animation_frame];
    asset.size.x = asset.size.x * sprite.scale.x;
    asset.size.y = asset.size.y * sprite.scale.y;

    graphics_draw_texture(renderer, asset, pos, ort);
}

void graphics_add_animated_sprite(Entity entity, AnimatedSprite sprite) {
    animated_sprites[entity] = sprite;
    entity_mask[entity] |= ANIMATED_SPRITE;
}

void graphics_draw_animated_sprites(SDL_Renderer *renderer) {
    CMask filter = ANIMATED_SPRITE;
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(entity_has_components(i, filter)) {
            graphics_draw_sprite(renderer, animated_sprites[i], positions[i], orientations[i]);
        }
    }
}

void graphics_update_sprite_frames(Tick current_tick, Time current_time) {
    CMask filter = ANIMATED_SPRITE;
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(entity_has_components(i, filter)) {
            graphics_update_sprite_frame(&animated_sprites[i], current_tick, current_time);
        }
    }
}

