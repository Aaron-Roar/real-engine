#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "graphics.h"
#include "console.h"
#include "engine.h"
#include "math2d.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

const Color background_color = (Color){255,255,255,255};
#define amount_of_entities 20
Entity selection = 0;
Entity selected_entity = 0;
//Selection Info

typedef enum {
    EDITOR_NONE,

    EDITOR_SELECT,

    EDITOR_MOVE,
    EDITOR_MOVE_CENTER,

    EDITOR_ROTATE,
    EDITOR_ROTATE_CW,
    EDITOR_ROTATE_CCW,

    EDITOR_SCALE,
    EDITOR_SCALE_X,
    EDITOR_SCALE_Y,
} LevelEditorMode;


typedef enum {
    KEY_NONE,
    KEY_0,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_BACKSPACE,
    KEY_DELETE,
    KEY_ENTER,
    KEY_EQUALS,
    KEY_ESCAPE,

    KEY_ALT,
    KEY_CTRL,
    KEY_SHIFT,

    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    KEY_MINUS,
    KEY_PLUS,

    KEY_MOUSE_LEFT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_RIGHT,

    KEY_SPACE,
    KEY_TAB,
} EditorInputKey;

typedef struct {
    EditorInputKey key;
    Vec2D mouse_position;
    bool pressed;
    bool shift;
    bool ctrl;
    bool alt;
    bool super;
} EditorInput;

Vec2D get_mouse_coordinates(SDL_Renderer *renderer) {
    float window_x, window_y;
    SDL_GetMouseState(&window_x, &window_y);
    float render_x, render_y;
    SDL_RenderCoordinatesFromWindow(
        renderer,
        window_x,
        window_y,
        &render_x,
        &render_y
    );
    return (Vec2D) {render_x, render_y};
}

EditorInput sdl_event_to_editor_input(SDL_Renderer *renderer, SDL_Event event)
{
    EditorInput input = {
        .key = KEY_NONE,
        .mouse_position = get_mouse_coordinates(renderer),
        .pressed = false,
        .shift = false,
        .ctrl = false,
        .alt = false,
        .super = false,
    };

    /*
     * Keyboard input
     */
    if (event.type == SDL_EVENT_KEY_DOWN ||
        event.type == SDL_EVENT_KEY_UP) {

        input.pressed = (event.type == SDL_EVENT_KEY_DOWN);

        input.shift = (event.key.mod & SDL_KMOD_SHIFT) != 0;
        input.ctrl  = (event.key.mod & SDL_KMOD_CTRL)  != 0;
        input.alt   = (event.key.mod & SDL_KMOD_ALT)   != 0;
        input.super = (event.key.mod & SDL_KMOD_GUI)   != 0;

        switch (event.key.scancode) {
            case SDL_SCANCODE_0: input.key = KEY_0; break;
            case SDL_SCANCODE_1: input.key = KEY_1; break;
            case SDL_SCANCODE_2: input.key = KEY_2; break;
            case SDL_SCANCODE_3: input.key = KEY_3; break;
            case SDL_SCANCODE_4: input.key = KEY_4; break;
            case SDL_SCANCODE_5: input.key = KEY_5; break;
            case SDL_SCANCODE_6: input.key = KEY_6; break;
            case SDL_SCANCODE_7: input.key = KEY_7; break;
            case SDL_SCANCODE_8: input.key = KEY_8; break;
            case SDL_SCANCODE_9: input.key = KEY_9; break;

            case SDL_SCANCODE_A: input.key = KEY_A; break;
            case SDL_SCANCODE_B: input.key = KEY_B; break;
            case SDL_SCANCODE_C: input.key = KEY_C; break;
            case SDL_SCANCODE_D: input.key = KEY_D; break;
            case SDL_SCANCODE_E: input.key = KEY_E; break;
            case SDL_SCANCODE_F: input.key = KEY_F; break;
            case SDL_SCANCODE_G: input.key = KEY_G; break;
            case SDL_SCANCODE_H: input.key = KEY_H; break;
            case SDL_SCANCODE_I: input.key = KEY_I; break;
            case SDL_SCANCODE_J: input.key = KEY_J; break;
            case SDL_SCANCODE_K: input.key = KEY_K; break;
            case SDL_SCANCODE_L: input.key = KEY_L; break;
            case SDL_SCANCODE_M: input.key = KEY_M; break;
            case SDL_SCANCODE_N: input.key = KEY_N; break;
            case SDL_SCANCODE_O: input.key = KEY_O; break;
            case SDL_SCANCODE_P: input.key = KEY_P; break;
            case SDL_SCANCODE_Q: input.key = KEY_Q; break;
            case SDL_SCANCODE_R: input.key = KEY_R; break;
            case SDL_SCANCODE_S: input.key = KEY_S; break;
            case SDL_SCANCODE_T: input.key = KEY_T; break;
            case SDL_SCANCODE_U: input.key = KEY_U; break;
            case SDL_SCANCODE_V: input.key = KEY_V; break;
            case SDL_SCANCODE_W: input.key = KEY_W; break;
            case SDL_SCANCODE_X: input.key = KEY_X; break;
            case SDL_SCANCODE_Y: input.key = KEY_Y; break;
            case SDL_SCANCODE_Z: input.key = KEY_Z; break;

            case SDL_SCANCODE_BACKSPACE:
                input.key = KEY_BACKSPACE;
                break;

            case SDL_SCANCODE_DELETE:
                input.key = KEY_DELETE;
                break;

            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_KP_ENTER:
                input.key = KEY_ENTER;
                break;

            case SDL_SCANCODE_EQUALS:
                input.key = KEY_EQUALS;
                break;

            case SDL_SCANCODE_ESCAPE:
                input.key = KEY_ESCAPE;
                break;

            case SDL_SCANCODE_LALT:
            case SDL_SCANCODE_RALT:
                input.key = KEY_ALT;
                break;

            case SDL_SCANCODE_LCTRL:
            case SDL_SCANCODE_RCTRL:
                input.key = KEY_CTRL;
                break;

            case SDL_SCANCODE_LSHIFT:
            case SDL_SCANCODE_RSHIFT:
                input.key = KEY_SHIFT;
                break;

            case SDL_SCANCODE_UP:
                input.key = KEY_UP;
                break;

            case SDL_SCANCODE_DOWN:
                input.key = KEY_DOWN;
                break;

            case SDL_SCANCODE_LEFT:
                input.key = KEY_LEFT;
                break;

            case SDL_SCANCODE_RIGHT:
                input.key = KEY_RIGHT;
                break;

            case SDL_SCANCODE_MINUS:
            case SDL_SCANCODE_KP_MINUS:
                input.key = KEY_MINUS;
                break;

            case SDL_SCANCODE_KP_PLUS:
                input.key = KEY_PLUS;
                break;

            case SDL_SCANCODE_SPACE:
                input.key = KEY_SPACE;
                break;

            case SDL_SCANCODE_TAB:
                input.key = KEY_TAB;
                break;

            default:
                input.key = KEY_NONE;
                break;
        }

        return input;
    }

    /*
     * Mouse input
     */
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
        event.type == SDL_EVENT_MOUSE_BUTTON_UP) {

        input.pressed = (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);

        switch (event.button.button) {
            case SDL_BUTTON_LEFT:
                input.key = KEY_MOUSE_LEFT;
                break;

            case SDL_BUTTON_MIDDLE:
                input.key = KEY_MOUSE_MIDDLE;
                break;

            case SDL_BUTTON_RIGHT:
                input.key = KEY_MOUSE_RIGHT;
                break;

            default:
                input.key = KEY_NONE;
                break;
        }

        /*
         * Mouse events don't contain keyboard modifier information,
         * so query the current modifier state.
         */
        SDL_Keymod modifiers = SDL_GetModState();

        input.shift = (modifiers & SDL_KMOD_SHIFT) != 0;
        input.ctrl  = (modifiers & SDL_KMOD_CTRL)  != 0;
        input.alt   = (modifiers & SDL_KMOD_ALT)   != 0;
        input.super = (modifiers & SDL_KMOD_GUI)   != 0;

        return input;
    }

    return input;
}

LevelEditorMode editor_input_to_mode(LevelEditorMode current_mode, EditorInput input) {

    //Check MODE DEDICATED KEYS
    switch(input.key) {
        case KEY_R:
            return EDITOR_ROTATE;
        case KEY_S:
            return EDITOR_SCALE;
        case KEY_M:
            return EDITOR_MOVE;
        default:
            break;
    }

    //Check MODE dependent keys
    switch(current_mode) {
        case EDITOR_SELECT:
            switch (input.key) {
                case KEY_M:
                    return EDITOR_MOVE;

                case KEY_R:
                    return EDITOR_ROTATE;

                case KEY_S:
                    return EDITOR_SCALE;

                default:
                    return EDITOR_SELECT;
            }

        case EDITOR_MOVE:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_SPACE:
                    return EDITOR_MOVE_CENTER;
                default:
                    return EDITOR_MOVE;
            }
        case EDITOR_MOVE_CENTER:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_MOVE;
                default:
                    return EDITOR_MOVE_CENTER;
            }

        case EDITOR_ROTATE:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_LEFT:
                    return EDITOR_ROTATE_CCW;
                case KEY_RIGHT:
                    return EDITOR_ROTATE_CW;
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_CW:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_ROTATE;
                case KEY_LEFT:
                    return EDITOR_ROTATE_CCW;
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_CCW:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_ROTATE;
                case KEY_RIGHT:
                    return EDITOR_ROTATE_CW;
                default:
                    return EDITOR_ROTATE;
            }

        case EDITOR_SCALE:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_UP:
                    return EDITOR_SCALE_Y;
                case KEY_DOWN:
                    return EDITOR_SCALE_Y;
                case KEY_LEFT:
                    return EDITOR_SCALE_X;
                case KEY_RIGHT:
                    return EDITOR_SCALE_X;
                default:
                    return EDITOR_SCALE;
            }

        case EDITOR_SCALE_X:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_UP:
                    return EDITOR_SCALE_Y;
                case KEY_DOWN:
                    return EDITOR_SCALE_Y;
                case KEY_LEFT:
                    return EDITOR_SCALE_X;
                case KEY_RIGHT:
                    return EDITOR_SCALE_X;
                default:
                    return EDITOR_SCALE_X;
            }

        case EDITOR_SCALE_Y:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_UP:
                    return EDITOR_SCALE_Y;
                case KEY_DOWN:
                    return EDITOR_SCALE_Y;
                case KEY_LEFT:
                    return EDITOR_SCALE_X;
                case KEY_RIGHT:
                    return EDITOR_SCALE_X;
                default:
                    return EDITOR_SCALE_Y;
            }

        default:
            return EDITOR_SELECT;
    }
    return current_mode;
}



void editor_select(EditorInput input) {
    if(input.pressed) {
        //Do something here??
        console_write(LOG_APP, "Pressed\n");
        for(int i = 0; i < MAX_ENTITIES; i += 1) {
            if(i == selection) {
                continue;
            }
            if(collision_reports[selection].collisions[i]) {
                selected_entity = i;
                console_write(LOG_APP, "Selected Entity: %d\n", i);
            }
        }
    }
}

void editor_move(EditorInput input) {
    positions[selected_entity] = positions[selection];
}

void editor_rotate_cw(EditorInput input) {
    set_orientation(selected_entity, orientations[selected_entity] + 10*2*PI_F/360);
}

void editor_rotate_ccw(EditorInput input) {
    set_orientation(selected_entity, orientations[selected_entity] - 10*2*PI_F/360);
}

void resolve_mode(LevelEditorMode mode, EditorInput input) {
    switch(mode) {
        case EDITOR_NONE:
            break;
        case EDITOR_SELECT:
            editor_select(input);
            break;
        case EDITOR_MOVE:
            editor_move(input);
            break;
        case EDITOR_ROTATE_CW:
            editor_rotate_cw(input);
            break;
        case EDITOR_ROTATE_CCW:
            editor_rotate_ccw(input);
        default:
            break;
    }
}

void bind_selection_to_mouse(SDL_Renderer *renderer) {
    positions[selection] = get_mouse_coordinates(renderer);
}

void print_mode(LevelEditorMode mode) {
    switch(mode) {
        case EDITOR_NONE:
            console_write(LOG_APP, "Mode is NONE\n");
            break;
        case EDITOR_SELECT:
            console_write(LOG_APP, "Mode is SELECT\n");
            break;
        case EDITOR_MOVE:
            console_write(LOG_APP, "Mode is MOVE\n");
            break;
        case EDITOR_MOVE_CENTER:
            console_write(LOG_APP, "Mode is MOVE_CENTER\n");
            break;
        case EDITOR_ROTATE:
            console_write(LOG_APP, "Mode is ROTATE\n");
            break;
        case EDITOR_ROTATE_CW:
            console_write(LOG_APP, "Mode is ROTATE_CW\n");
            break;
        case EDITOR_ROTATE_CCW:
            console_write(LOG_APP, "Mode is ROTATE_CCW\n");
            break;
        case EDITOR_SCALE:
            console_write(LOG_APP, "Mode is SCALE\n");
            break;
        case EDITOR_SCALE_X:
            console_write(LOG_APP, "Mode is SCALE_X\n");
            break;
        case EDITOR_SCALE_Y:
            console_write(LOG_APP, "Mode is SCALE_Y\n");
            break;
        default:
            console_write(LOG_APP, "Mode is not working\n");
            break;

    }
}


bool has_selection = false;
int main() {
    console_init();
    console_set_debug(CONSOLE_DEBUG_OFF);
    engine_init();
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    if (!graphics_start(&renderer, &window)) {
        engine_shutdown();
        return 1;
    }

    selection = add_entity();
    set_static(selection);
    set_position(selection, (Vec2D){80, 390});
    Shape selection_hit_box = create_circle(10, 5);
    set_hitbox(selection, selection_hit_box);

    Entity entity_1 = add_entity();
    set_static(entity_1);
    set_position(entity_1, (Vec2D){80, 390});
    set_orientation(entity_1, 0*(PI_F/180));
    Shape shape_1 = create_square(40, 150);
    set_hitbox(entity_1, shape_1);

    Entity entity_2 = add_entity();
    set_static(entity_2);
    set_position(entity_2, (Vec2D){80, 390});
    set_orientation(entity_2, 0*(PI_F/180));
    Shape shape_2 = create_circle(10, 20);
    set_hitbox(entity_2, shape_2);

    LevelEditorMode prev_mode = EDITOR_NONE;
    LevelEditorMode current_mode = EDITOR_NONE;
    //Game Loop
    while (console_is_active()) {
        clean_entities_past_lifetime();

        //Console
        ConsoleLogString console_line = {0};
        if(read_console(&console_line)) {
            console_write(LOG_CONSOLE, "%s", console_line.string);
        }

        SDL_Event event = engine_poll_event();
        EditorInput input = sdl_event_to_editor_input(renderer, event);
        prev_mode = current_mode;
        current_mode = editor_input_to_mode(current_mode, input);
        if(prev_mode != current_mode) {
            print_mode(current_mode);
        }
        bind_selection_to_mouse(renderer);
        resolve_mode(current_mode, input);

        //physics
        engine_update_time();
        engine_update_tick();
        //apply_collisions();
        system_update_physics(engine_get_dt());

        draw_background(renderer, background_color);

        draw_hit_boxes(renderer);
        update_sprite_frames(engine_get_tick(), engine_get_time());
        draw_animated_sprites(renderer);
        show_graphics(renderer);

    }
    graphics_end(renderer, window);
    engine_shutdown();
}

