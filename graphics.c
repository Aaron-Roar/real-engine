#include "graphics.h"
#include "console.h"
#include "engine.h"
#include "systems.h"
#include <stdio.h>

AnimatedSprite animated_sprites[MAX_ENTITIES] = {0};
const Color hit_box_color = (Color){255,0,0,255};

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

Position graphics_world_to_screen(Position world)
{
    return (Position){
        .x = world.x + WINDOW_WIDTH * 0.5f,
        .y = WINDOW_HEIGHT * 0.5f - world.y
    };
}

Position graphics_screen_to_world(Position screen)
{
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
    /* as you can see from this, rendering draws over whatever was drawn before it. */
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
    SDL_RenderClear(renderer);  /* start with a blank canvas. */
}

void graphics_draw_rect(SDL_Renderer *renderer, Shape rect, Position pos) {
    SDL_FRect sdl_rect;
    /* draw a filled rectangle in the middle of the canvas. */
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  /* blue, full alpha */
    Position screen_loc = graphics_world_to_screen(pos);
    sdl_rect.x = screen_loc.x;
    sdl_rect.y = screen_loc.y;
    sdl_rect.w = 20;
    sdl_rect.h = 20;
    SDL_RenderFillRect(renderer, &sdl_rect);
}

void graphics_show(SDL_Renderer *renderer) {
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

bool graphics_draw_shape_filled(SDL_Renderer *renderer, Shape shape, Color color)
{
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
    Shape shape = get_global_hit_box(entity);
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

TextureAsset graphics_load_texture(SDL_Renderer *renderer, TextureDescriptor text_desc) {
        SDL_Surface *surface = NULL;
        char *png_path = NULL;
        TextureAsset asset = {0};
        asset.size = (Scale){
            .x = text_desc.size.x,
            .y = text_desc.size.y,
        };

        SDL_asprintf(&png_path, "%s", text_desc.file);  /* allocate a string of the full file path */
        surface = SDL_LoadPNG(png_path);
        SDL_free(png_path);  /* done with this, the file is loaded. */

        asset.texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */

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
    //SDL_RenderTexture(renderer, texture_asset.texture, NULL, &dst_rect);
    double degrees = (double)ort * 180.0 / (double)PI_F;
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
        if(has_components(i, filter)) {
            graphics_draw_sprite(renderer, animated_sprites[i], positions[i], orientations[i]);
        }
    }
}

void graphics_update_sprite_frames(Tick current_tick, Time current_time) {
    CMask filter = ANIMATED_SPRITE;
    for(int i = 0; i < MAX_ENTITIES; i += 1) {
        if(has_components(i, filter)) {
            graphics_update_sprite_frame(&animated_sprites[i], current_tick, current_time);
        }
    }
}
