#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_main.h>
#include "entity_components.h"
#include "engine.h"
#include "physics.h"
#include "math2d.h"
#include "math.h"

extern SDL_Renderer *sdl_renderer;
extern SDL_Window *sdl_window;
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define MAX_TEXTURES 50
#define MAX_ANIMATIONS_FRAMES 20
#define MAX_ANIMATION_SETS 10

#define RECORDING_WIDTH  WINDOW_WIDTH
#define RECORDING_HEIGHT WINDOW_HEIGHT

typedef struct {
    float x;
    float y;
} Scale;

typedef struct {
  const char *file;
  Scale size;
} TextureDescriptor;

typedef struct {
  TextureDescriptor texture_descriptors[MAX_ANIMATIONS_FRAMES];
  uint8_t amount_of_descriptors;
  Tick ticks_per_frame;
  Time time_per_frame;
} AnimationDescriptor;

typedef SDL_Texture* Texture;

typedef struct {
    Texture texture;
    Scale size;
} TextureAsset;

typedef struct {
    TextureAsset textures[MAX_TEXTURES];
    int amount;
} TextureList;

typedef struct {
    TextureList texture_list;
    Tick ticks_per_frame;
    Time time_per_frame;
} AnimationAsset;

typedef enum {DIRECTION_LEFT, DIRECTION_RIGHT} Direction;


typedef struct {
    AnimationAsset animation;
    int animation_frame;
    Tick last_update_tick;
    Time last_update_time;
    Direction direction;
    Scale scale;
} AnimatedSprite;

typedef struct {
    AnimatedSprite sprite_set[MAX_ANIMATION_SETS];
    uint8_t amount_of_sets;
} AnimatedSpriteSet;

MEMORY_DECLARE_OBJECT_POOL(AnimatedSpritePool, AnimatedSprite);
extern AnimatedSpritePool animated_sprites_pool;
#define animated_sprites animated_sprites_pool.objects

typedef enum Fill {
    GRAPHICS_OUTLINE,
    GRAPHICS_FILLED
} Fill;

typedef struct Color {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t alpha;
} Color;

Color graphics_creat_color_hex(uint32_t hex_color_code);
Color graphics_creat_color_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
bool graphics_tables_init(void);
void graphics_tables_destroy(void);
bool graphics_start();
void graphics_end();
bool graphics_poll_events(SDL_Event *event);
void graphics_draw_background(Color color);
void graphics_show();
bool graphics_draw_shape_outline(Shape shape, Color color);
bool graphics_draw_shape_filled(Shape shape, Color color);
void graphics_draw_hit_box(Entity entity, Fill fill_type);
void graphics_draw_hit_boxes();
TextureAsset graphics_load_texture(TextureDescriptor text_desc);
AnimationAsset graphics_load_animation(AnimationDescriptor anim_desc);
AnimatedSprite graphics_create_animated_sprite(AnimationAsset asset_ptr, Scale scale);
void graphics_update_sprite_frame(AnimatedSprite *sprite, Tick current_tick, Time current_time);
void graphics_draw_texture(TextureAsset texture_asset, Position pos, Orientation ort);
void graphics_draw_sprite(AnimatedSprite sprite, Position pos, Orientation ort);
void graphics_add_animated_sprite(Entity entity, AnimatedSprite sprite);
void graphics_draw_animated_sprites();
void graphics_update_sprite_frames(Tick current_tick, Time current_time);
void graphics_scale_textures(Entity entity, Scale scale);
Position graphics_world_to_screen(Position pos);
Position graphics_screen_to_world(Position screen);
void graphics_draw_grid();
bool graphics_recording_start(
    const char *output_path,
    int fps
);
void graphics_recording_stop(void);
void graphics_draw_particles();
void graphics_draw_local_origin(Entity entity);
void graphics_draw_local_origins();
#endif
