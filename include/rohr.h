#ifndef ROHR_H
#define ROHR_H

#include "console.h"
#include "controller.h"
#include "engine.h"
#include "entity_components.h"
#include "error.h"
#include "graphics.h"
#include "grid.h"
#include "level_editor.h"
#include "math2d.h"
#include "physics.h"
#include "systems.h"
#include "tools.h"

/**
 * @file rohr.h
 * Public Rohr Engine API facade.
 *
 * Applications can include this file to use the engine through rohr_-prefixed
 * functions while the implementation keeps its smaller internal modules.
 */

/** Rohr-prefixed public API wrapper. */
EngineResult rohr_engine_init(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_shutdown(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_update_time(void);
/** Rohr-prefixed public API wrapper. */
Time rohr_engine_get_time(void);
/** Rohr-prefixed public API wrapper. */
Tick rohr_engine_get_tick(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_pause(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_resume(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_update_tick(void);
/** Rohr-prefixed public API wrapper. */
SDL_Event rohr_engine_poll_event(void);
/** Rohr-prefixed public API wrapper. */
bool rohr_engine_is_paused(void);
/** Rohr-prefixed public API wrapper. */
Time rohr_engine_get_dt(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_calculate_dt(void);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_set_dt(Time dt);
/** Rohr-prefixed public API wrapper. */
void rohr_engine_reset_clock(void);

/** Rohr-prefixed public API wrapper. */
EngineResult rohr_error_result_value(bool value);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_error_result_error(EngineError error);
/** Return true when a Rohr result value contains an error. */
#define rohr_error_check(ResultValue) error_check(ResultValue)
/** Rohr-prefixed public API wrapper. */
const char *rohr_error_default_message(EngineError error);
/** Rohr-prefixed public API wrapper. */
const char *rohr_error_string(EngineError error);

/** Rohr-prefixed public API wrapper. */
void rohr_console_print_logs(void);
/** Rohr-prefixed public API wrapper. */
void rohr_console_init(void);
/** Rohr-prefixed public API wrapper. */
void rohr_console_shutdown(void);
/** Rohr-prefixed public API wrapper. */
bool rohr_console_read(ConsoleLogString *input);
/** Rohr-prefixed public API wrapper. */
void rohr_console_write(LogSourceType source, const char *fmt, ...);
/** Rohr-prefixed public API wrapper. */
bool rohr_console_is_active(void);
/** Rohr-prefixed public API wrapper. */
void rohr_console_debug_write(LogSourceType source, const char *fmt, ...);
/** Rohr-prefixed public API wrapper. */
void rohr_console_set_debug(bool state);

/** Rohr-prefixed public API wrapper. */
bool rohr_entity_is_alive(Entity entity);
/** Rohr-prefixed public API wrapper. */
bool rohr_entity_index_is_alive(EntityIndex index);
/** Rohr-prefixed public API wrapper. */
bool rohr_entity_get_index(Entity entity, EntityIndex *index);
/** Rohr-prefixed public API wrapper. */
EntityResult rohr_entity_from_index(EntityIndex index);
/** Rohr-prefixed public API wrapper. */
EntityResult rohr_entity_add(void);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_delete(Entity entity);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_add_components(Entity entity, CMask mask);
/** Rohr-prefixed public API wrapper. */
bool rohr_entity_has_components(Entity entity, CMask components);
/** Rohr-prefixed public API wrapper. */
bool rohr_entity_index_has_components(EntityIndex index, CMask components);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_delete_components(Entity entity, CMask mask);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_set_child(Entity parent, Entity child);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_set_parent(Entity child, Entity parent);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_remove_parent(Entity child);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_remove_child(Entity parent, Entity child);
/** Rohr-prefixed public API wrapper. */
ChildrenResult rohr_entity_get_children(Entity entity);
/** Rohr-prefixed public API wrapper. */
ParentResult rohr_entity_get_parent(Entity entity);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_set_life_time(Entity entity, Time expirey_time, Tick expirey_tick);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_entity_remove_life_time(Entity entity);

/** Rohr-prefixed public API wrapper. */
Shape rohr_physics_shape_world_translate(Shape shape, Position position, Orientation angle);
/** Rohr-prefixed public API wrapper. */
float rohr_physics_polygon_moment_of_inertia(Shape shape, Mass mass_value);
/** Rohr-prefixed public API wrapper. */
Collision rohr_physics_sat_collision(Shape shape_1, Shape shape_2);
/** Rohr-prefixed public API wrapper. */
Vec1D rohr_physics_circle_moment_of_inertia(Shape circle, Mass mass_value);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_acceleration(Entity entity, Acceleration a);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_velocity(Entity entity, Velocity v);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_position(Entity entity, Position p);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_mass(Entity entity, Mass m);
/** Rohr-prefixed public API wrapper. */
EntityResult rohr_physics_set_force(Entity entity, Force f);
/** Rohr-prefixed public API wrapper. */
EntityResult rohr_physics_set_torque(Entity entity, Torque t);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_hitbox(Entity entity, Shape hitbox);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_orientation(Entity entity, Orientation angle);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_angular_velocity(Entity entity, AngularVelocity v);
/** Rohr-prefixed public API wrapper. */
ShapeResult rohr_physics_get_global_hit_box(Entity entity);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_restitution(Entity entity, Restitution restitution);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_dynamic(Entity entity);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_static(Entity entity);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_angle_lock(Entity entity, Orientation min, Orientation max);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_axis_lock(Entity entity, Axis axis, Position axis_point);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_friction(Entity entity, float friction);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_transform_lock(
    Entity driven,
    Entity driver,
    Vec2D local_offset,
    Orientation local_angle,
    bool lock_position,
    bool lock_orientation,
    bool inherit_velocity
);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_remove_transform_lock(Entity entity);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_transform_lock_current_transform(
    Entity driven,
    Entity driver,
    bool lock_position,
    bool lock_orientation,
    bool inherit_velocity
);
/** Rohr-prefixed public API wrapper. */
EntityResult rohr_physics_set_joint(
    Entity a,
    Entity b,
    JointType type,
    Vec2D local_anchor_a,
    Vec2D local_anchor_b,
    float stiffness,
    float damping
);
/** Rohr-prefixed public API wrapper. */
Collision rohr_physics_particle_collision(Shape shape_1, Shape shape_2);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_physics_set_collision_report(Entity entity, Entity target, bool state);
/** Rohr-prefixed public API wrapper. */
bool rohr_physics_get_collision_report(Entity entity, Entity target);

/** Rohr-prefixed public API wrapper. */
Color rohr_graphics_create_color_hex(uint32_t hex_color_code);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_graphics_start(void);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_end(void);
/** Rohr-prefixed public API wrapper. */
bool rohr_graphics_poll_events(SDL_Event *event);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_background(Color color);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_show(void);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_hit_box(Entity entity, Fill fill_type);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_hit_boxes(void);
/** Rohr-prefixed public API wrapper. */
TextureAssetResult rohr_graphics_load_texture(TextureDescriptor text_desc);
/** Rohr-prefixed public API wrapper. */
AnimationAssetResult rohr_graphics_load_animation(AnimationDescriptor anim_desc);
/** Rohr-prefixed public API wrapper. */
AnimatedSprite rohr_graphics_create_animated_sprite(AnimationAsset asset_ptr, Scale scale);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_graphics_add_animated_sprite(Entity entity, AnimatedSprite sprite);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_animated_sprites(void);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_update_sprite_frames(Tick current_tick, Time current_time);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_scale_textures(Entity entity, Scale scale);
/** Rohr-prefixed public API wrapper. */
Position rohr_graphics_world_to_screen(Position pos);
/** Rohr-prefixed public API wrapper. */
Position rohr_graphics_screen_to_world(Position screen);
/** Rohr-prefixed public API wrapper. */
Position rohr_graphics_get_mouse_screen_position(void);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_grid(void);
/** Rohr-prefixed public API wrapper. */
bool rohr_graphics_recording_start(const char *output_path, int fps);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_particles(void);
/** Rohr-prefixed public API wrapper. */
void rohr_graphics_draw_local_origins(void);

/** Rohr-prefixed public API wrapper. */
Vec2DList rohr_math_create_normals(Shape shape);
/** Rohr-prefixed public API wrapper. */
Vec2D rohr_math_normalize_vector(Vec2D vector);
/** Rohr-prefixed public API wrapper. */
Vec2DList rohr_math_normalize_vectors(Vec2DList vectors);
/** Rohr-prefixed public API wrapper. */
float rohr_math_dot_product(Vec2D vector_1, Vec2D vector_2);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_create_square(float width, float height);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_create_circle(float radius, uint8_t verticies);
/** Rohr-prefixed public API wrapper. */
Projection rohr_math_project_shape_on_axis(Shape shape, Axis axis);
/** Rohr-prefixed public API wrapper. */
float rohr_math_cross_2d(Vec2D a, Vec2D b);
/** Rohr-prefixed public API wrapper. */
Vec2D rohr_math_angular_velocity_cross_vec(float omega, Vec2D r);
/** Rohr-prefixed public API wrapper. */
Vec2D rohr_math_project_onto_axis(Vec2D v, Axis axis);
/** Rohr-prefixed public API wrapper. */
float rohr_math_axis_magnitude(Axis axis);
/** Rohr-prefixed public API wrapper. */
float rohr_math_vector_magnitude(Vec2D vector);
/** Rohr-prefixed public API wrapper. */
Vec2D rohr_math_rotate_vector(Vec2D vector, float angle);
/** Rohr-prefixed public API wrapper. */
Vec1D rohr_math_circle_radius(Shape circle, Vec2D centroid);
/** Rohr-prefixed public API wrapper. */
Vec2D rohr_math_vector_subtract(Vec2D vector_a, Vec2D vector_b);
/** Rohr-prefixed public API wrapper. */
Vec1D rohr_math_circle_overlap_depth(Vec2D centroid_1, Vec1D radius_1, Vec2D centroid_2, Vec1D radius_2);
/** Rohr-prefixed public API wrapper. */
float rohr_math_projection_overlap(Projection projection_1, Projection projection_2);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_scale_shape(Shape shape, float scale);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_scale_shape_y(Shape shape, float scale);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_scale_shape_x(Shape shape, float scale);
/** Rohr-prefixed public API wrapper. */
Vec2D rohr_math_polygon_centroid(Shape shape);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_add_vertex(Shape shape);
/** Rohr-prefixed public API wrapper. */
Shape rohr_math_delete_vertex(Shape shape);
/** Rohr-prefixed public API wrapper. */
AABB rohr_math_create_aabb(Shape world_shape);

/** Rohr-prefixed public API wrapper. */
void rohr_system_update_physics(double dt);
/** Rohr-prefixed public API wrapper. */
void rohr_system_clean_entities_past_lifetime(void);

/** Rohr-prefixed public API wrapper. */
EngineResult rohr_level_editor_init(void);
/** Rohr-prefixed public API wrapper. */
EngineResult rohr_level_editor_update(void);

/** Rohr-prefixed public API wrapper. */
void rohr_controller_print_keyboard_event(KeyboardEvent event);
/** Rohr-prefixed public API wrapper. */
void rohr_controller_update_key_states(KeyboardState *keyboard);
/** Rohr-prefixed public API wrapper. */
void rohr_controller_add_key_event(KeyboardState *keyboard, KeyboardEvent key_event);
/** Rohr-prefixed public API wrapper. */
KeyboardEvent rohr_controller_capture_keyboard_event(const SDL_Event *sdl_event);
/** Rohr-prefixed public API wrapper. */
void rohr_controller_print_mouse_event(MouseEvent event);
/** Rohr-prefixed public API wrapper. */
void rohr_controller_update_mouse_states(MouseState *mouse);
/** Rohr-prefixed public API wrapper. */
void rohr_controller_add_mouse_event(MouseState *mouse, MouseEvent mouse_event);
/** Rohr-prefixed public API wrapper. */
MouseEvent rohr_controller_capture_mouse_event(const SDL_Event *sdl_event);

/** Rohr-prefixed public API wrapper. */
void rohr_grid_add_entity_to_grids(Entity entity);
/** Rohr-prefixed public API wrapper. */
bool rohr_grid_checked_pair(Entity entity_1, Entity entity_2);
/** Rohr-prefixed public API wrapper. */
void rohr_grid_add_pair(Entity entity_1, Entity entity_2);
/** Rohr-prefixed public API wrapper. */
void rohr_grid_clear(void);
/** Rohr-prefixed public API wrapper. */
void rohr_grid_update_aabb(Entity entity);

/** Rohr-prefixed public API wrapper. */
void rohr_tools_delay(int seconds);
/** Rohr-prefixed public API wrapper. */
void rohr_tools_binary_to_string(uint32_t value, char *buffer, size_t size);
/** Rohr-prefixed public API wrapper. */
void rohr_tools_append_string(char *src, char *dst, size_t src_size, size_t dst_size);
/** Rohr-prefixed public API wrapper. */
uint32_t rohr_tools_sizeof_string(char *str, char delimiter);
/** Rohr-prefixed public API wrapper. */
int rohr_tools_random_range(int min, int max);
/** Rohr-prefixed public API wrapper. */
float rohr_tools_random_range_float(float min, float max);

#endif
