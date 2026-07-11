#include "entity_components.h"
#include "systems.h"
#include "tools.h"
#include "console.h"
#include "engine.h"
#include "math2d.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "examples/test-assets/elder-fly/elderfly_descriptors.h"

Entity selection = 0;
Entity selected_entity = 0;
//Selection Info

#define SCALE_INCREMENT 0.1
typedef enum {
    EDITOR_NONE,

    EDITOR_SELECT,

    EDITOR_MOVE,
    EDITOR_MOVE_CENTER,

    EDITOR_ROTATE,
    EDITOR_ROTATE_CW,
    EDITOR_ROTATE_CCW,
    EDITOR_ROTATE_UP,
    EDITOR_ROTATE_DOWN,
    EDITOR_ROTATE_LEFT,
    EDITOR_ROTATE_RIGHT,


    EDITOR_SCALE,
    EDITOR_SCALE_ALL_UP,
    EDITOR_SCALE_ALL_DOWN,
    EDITOR_SCALE_X_UP,
    EDITOR_SCALE_Y_UP,
    EDITOR_SCALE_X_DOWN,
    EDITOR_SCALE_Y_DOWN,
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

    KEY_SHIFT_A,
    KEY_SHIFT_B,
    KEY_SHIFT_C,
    KEY_SHIFT_D,
    KEY_SHIFT_E,
    KEY_SHIFT_F,
    KEY_SHIFT_G,
    KEY_SHIFT_H,
    KEY_SHIFT_I,
    KEY_SHIFT_J,
    KEY_SHIFT_K,
    KEY_SHIFT_L,
    KEY_SHIFT_M,
    KEY_SHIFT_N,
    KEY_SHIFT_O,
    KEY_SHIFT_P,
    KEY_SHIFT_Q,
    KEY_SHIFT_R,
    KEY_SHIFT_S,
    KEY_SHIFT_T,
    KEY_SHIFT_U,
    KEY_SHIFT_V,
    KEY_SHIFT_W,
    KEY_SHIFT_X,
    KEY_SHIFT_Y,
    KEY_SHIFT_Z,

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
    KEY_SHIFT_UP,
    KEY_SHIFT_DOWN,
    KEY_SHIFT_LEFT,
    KEY_SHIFT_RIGHT,

    KEY_MINUS,
    KEY_PLUS,

    KEY_MOUSE_LEFT,
    KEY_MOUSE_MIDDLE,
    KEY_MOUSE_RIGHT,

    KEY_SPACE,
    KEY_SHIFT_SPACE,
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

        if(input.shift) {
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

                case SDL_SCANCODE_A: input.key = KEY_SHIFT_A; break;
                case SDL_SCANCODE_B: input.key = KEY_SHIFT_B; break;
                case SDL_SCANCODE_C: input.key = KEY_SHIFT_C; break;
                case SDL_SCANCODE_D: input.key = KEY_SHIFT_D; break;
                case SDL_SCANCODE_E: input.key = KEY_SHIFT_E; break;
                case SDL_SCANCODE_F: input.key = KEY_SHIFT_F; break;
                case SDL_SCANCODE_G: input.key = KEY_SHIFT_G; break;
                case SDL_SCANCODE_H: input.key = KEY_SHIFT_H; break;
                case SDL_SCANCODE_I: input.key = KEY_SHIFT_I; break;
                case SDL_SCANCODE_J: input.key = KEY_SHIFT_J; break;
                case SDL_SCANCODE_K: input.key = KEY_SHIFT_K; break;
                case SDL_SCANCODE_L: input.key = KEY_SHIFT_L; break;
                case SDL_SCANCODE_M: input.key = KEY_SHIFT_M; break;
                case SDL_SCANCODE_N: input.key = KEY_SHIFT_N; break;
                case SDL_SCANCODE_O: input.key = KEY_SHIFT_O; break;
                case SDL_SCANCODE_P: input.key = KEY_SHIFT_P; break;
                case SDL_SCANCODE_Q: input.key = KEY_SHIFT_Q; break;
                case SDL_SCANCODE_R: input.key = KEY_SHIFT_R; break;
                case SDL_SCANCODE_S: input.key = KEY_SHIFT_S; break;
                case SDL_SCANCODE_T: input.key = KEY_SHIFT_T; break;
                case SDL_SCANCODE_U: input.key = KEY_SHIFT_U; break;
                case SDL_SCANCODE_V: input.key = KEY_SHIFT_V; break;
                case SDL_SCANCODE_W: input.key = KEY_SHIFT_W; break;
                case SDL_SCANCODE_X: input.key = KEY_SHIFT_X; break;
                case SDL_SCANCODE_Y: input.key = KEY_SHIFT_Y; break;
                case SDL_SCANCODE_Z: input.key = KEY_SHIFT_Z; break;

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
                    input.key = KEY_SHIFT_UP;
                    break;

                case SDL_SCANCODE_DOWN:
                    input.key = KEY_SHIFT_DOWN;
                    break;

                case SDL_SCANCODE_LEFT:
                    input.key = KEY_SHIFT_LEFT;
                    break;

                case SDL_SCANCODE_RIGHT:
                    input.key = KEY_SHIFT_RIGHT;
                    break;

                case SDL_SCANCODE_MINUS:
                case SDL_SCANCODE_KP_MINUS:
                    input.key = KEY_MINUS;
                    break;

                case SDL_SCANCODE_KP_PLUS:
                    input.key = KEY_PLUS;
                    break;

                case SDL_SCANCODE_SPACE:
                    input.key = KEY_SHIFT_SPACE;
                    break;

                case SDL_SCANCODE_TAB:
                    input.key = KEY_TAB;
                    break;

                default:
                    input.key = KEY_NONE;
                    break;
            }

        }
        else {
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

                case KEY_M:
                    return EDITOR_MOVE;
                case KEY_R:
                    return EDITOR_ROTATE;
                case KEY_S:
                    return EDITOR_SCALE;
                default:
                    return EDITOR_MOVE;
            }

        case EDITOR_ROTATE:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_LEFT:
                    return EDITOR_ROTATE_CCW;
                case KEY_RIGHT:
                    return EDITOR_ROTATE_CW;
                case KEY_SHIFT_UP:
                    return EDITOR_ROTATE_UP;
                case KEY_SHIFT_DOWN:
                    return EDITOR_ROTATE_DOWN;
                case KEY_SHIFT_RIGHT:
                    return EDITOR_ROTATE_RIGHT;
                case KEY_SHIFT_LEFT:
                    return EDITOR_ROTATE_LEFT;

                case KEY_M:
                    return EDITOR_MOVE;
                case KEY_R:
                    return EDITOR_ROTATE;
                case KEY_S:
                    return EDITOR_SCALE;

                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_CW:
            switch(input.key) {
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_CCW:
            switch(input.key) {
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_UP:
            switch(input.key) {
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_DOWN:
            switch(input.key) {
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_LEFT:
            switch(input.key) {
                default:
                    return EDITOR_ROTATE;
            }
        case EDITOR_ROTATE_RIGHT:
            switch(input.key) {
                default:
                    return EDITOR_ROTATE;
            }

        case EDITOR_SCALE:
            switch(input.key) {
                case KEY_ESCAPE:
                    return EDITOR_SELECT;
                case KEY_SPACE:
                    return EDITOR_SCALE_ALL_UP;
                case KEY_UP:
                    return EDITOR_SCALE_Y_UP;
                case KEY_DOWN:
                    return EDITOR_SCALE_Y_UP;
                case KEY_LEFT:
                    return EDITOR_SCALE_X_UP;
                case KEY_RIGHT:
                    return EDITOR_SCALE_X_UP;
                case KEY_SHIFT_SPACE:
                    return EDITOR_SCALE_ALL_DOWN;
                case KEY_SHIFT_UP:
                    return EDITOR_SCALE_Y_DOWN;
                case KEY_SHIFT_DOWN:
                    return EDITOR_SCALE_Y_DOWN;
                case KEY_SHIFT_LEFT:
                    return EDITOR_SCALE_X_DOWN;
                case KEY_SHIFT_RIGHT:
                    return EDITOR_SCALE_X_DOWN;

                case KEY_M:
                    return EDITOR_MOVE;
                case KEY_R:
                    return EDITOR_ROTATE;
                case KEY_S:
                    return EDITOR_SCALE;

                default:
                    return EDITOR_SCALE;
            }
        case EDITOR_SCALE_ALL_UP:
            switch(input.key) {
                default:
                    return EDITOR_SCALE;
            }
        case EDITOR_SCALE_ALL_DOWN:
            switch(input.key) {
                default:
                    return EDITOR_SCALE;
            }
        case EDITOR_SCALE_X_UP:
            switch(input.key) {
                default:
                    return EDITOR_SCALE;
            }
        case EDITOR_SCALE_X_DOWN:
            switch(input.key) {
                default:
                    return EDITOR_SCALE;
            }
        case EDITOR_SCALE_Y_UP:
            switch(input.key) {
                default:
                    return EDITOR_SCALE;
            }
        case EDITOR_SCALE_Y_DOWN:
            switch(input.key) {
                default:
                    return EDITOR_SCALE;
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

//Rotation
void editor_rotate_cw(EditorInput input) {
    set_orientation(selected_entity, orientations[selected_entity] + 1*2*PI_F/360);
}
void editor_rotate_ccw(EditorInput input) {
    set_orientation(selected_entity, orientations[selected_entity] - 1*2*PI_F/360);
}
void editor_rotate_up(EditorInput input) {
    set_orientation(selected_entity, 0*2*PI_F/360);
}
void editor_rotate_down(EditorInput input) {
    set_orientation(selected_entity, 180*2*PI_F/360);
}
void editor_rotate_right(EditorInput input) {
    set_orientation(selected_entity, 90*2*PI_F/360);
}
void editor_rotate_left(EditorInput input) {
    set_orientation(selected_entity, -90*2*PI_F/360);
}

void editor_scale_all_up(EditorInput input) {
    Shape shape = scale_shape(hit_boxes[selected_entity], 1 + SCALE_INCREMENT);
    set_hitbox(selected_entity, shape);
    scale_textures(selected_entity, (Scale){1 + SCALE_INCREMENT,1 + SCALE_INCREMENT});
}
void editor_scale_all_down(EditorInput input) {
    Shape shape = scale_shape(hit_boxes[selected_entity], 1 - SCALE_INCREMENT);
    set_hitbox(selected_entity, shape);
    scale_textures(selected_entity, (Scale){1 - SCALE_INCREMENT,1 - SCALE_INCREMENT});
}
void editor_scale_x_up(EditorInput input) {
    Shape shape = scale_shape_x(hit_boxes[selected_entity], 1 + SCALE_INCREMENT);
    set_hitbox(selected_entity, shape);
    scale_textures(selected_entity, (Scale){1 + SCALE_INCREMENT,1});
}
void editor_scale_x_down(EditorInput input) {
    Shape shape = scale_shape_x(hit_boxes[selected_entity], 1 - SCALE_INCREMENT);
    set_hitbox(selected_entity, shape);
    scale_textures(selected_entity, (Scale){1 - SCALE_INCREMENT,1});
}
void editor_scale_y_up(EditorInput input) {
    Shape shape = scale_shape_y(hit_boxes[selected_entity], 1 + SCALE_INCREMENT);
    set_hitbox(selected_entity, shape);
    scale_textures(selected_entity, (Scale){1,1 + SCALE_INCREMENT});
}
void editor_scale_y_down(EditorInput input) {
    Shape shape = scale_shape_y(hit_boxes[selected_entity], 1 - SCALE_INCREMENT);
    set_hitbox(selected_entity, shape);
    scale_textures(selected_entity, (Scale){1,1 - SCALE_INCREMENT});
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
            break;
        case EDITOR_ROTATE_UP:
            editor_rotate_up(input);
            break;
        case EDITOR_ROTATE_DOWN:
            editor_rotate_down(input);
            break;
        case EDITOR_ROTATE_LEFT:
            editor_rotate_left(input);
            break;
        case EDITOR_ROTATE_RIGHT:
            editor_rotate_right(input);
            break;
        case EDITOR_SCALE_ALL_UP:
            editor_scale_all_up(input);
            break;
        case EDITOR_SCALE_ALL_DOWN:
            editor_scale_all_down(input);
            break;
        case EDITOR_SCALE_X_UP:
            editor_scale_x_up(input);
            break;
        case EDITOR_SCALE_X_DOWN:
            editor_scale_x_down(input);
            break;
        case EDITOR_SCALE_Y_UP:
            editor_scale_y_up(input);
            break;
        case EDITOR_SCALE_Y_DOWN:
            editor_scale_y_down(input);
            break;
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
        case EDITOR_SCALE_X_UP:
            console_write(LOG_APP, "Mode is SCALE_X_UP\n");
            break;
        case EDITOR_SCALE_Y_UP:
            console_write(LOG_APP, "Mode is SCALE_Y_UP\n");
            break;
        case EDITOR_SCALE_X_DOWN:
            console_write(LOG_APP, "Mode is SCALE_X_DOWN\n");
            break;
        case EDITOR_SCALE_Y_DOWN:
            console_write(LOG_APP, "Mode is SCALE_Y_DOWN\n");
            break;
        case EDITOR_ROTATE_UP:
            console_write(LOG_APP, "Mode is ROTATE_UP\n");
            break;
        case EDITOR_ROTATE_DOWN:
            console_write(LOG_APP, "Mode is ROTATE_DOWN\n");
            break;
        case EDITOR_ROTATE_LEFT:
            console_write(LOG_APP, "Mode is ROTATE_LEFT\n");
            break;
        case EDITOR_ROTATE_RIGHT:
            console_write(LOG_APP, "Mode is ROTATE_RIGHT\n");
            break;
        default:
            console_write(LOG_APP, "Mode is not working\n");
            break;

    }
}


bool has_selection = false;
LevelEditorMode prev_mode = EDITOR_NONE;
LevelEditorMode current_mode = EDITOR_NONE;

void level_editor_init() {
    selection = add_entity();
    set_static(selection);
    set_position(selection, (Vec2D){80, 390});
    Shape selection_hit_box = create_circle(0.1, 5);
    set_hitbox(selection, selection_hit_box);
    delete_components(selection, COLLISION);
}

void level_editor_update(SDL_Renderer *renderer) {
        SDL_Event event = engine_poll_event();
        EditorInput input = sdl_event_to_editor_input(renderer, event);
        prev_mode = current_mode;
        current_mode = editor_input_to_mode(current_mode, input);
        if(prev_mode != current_mode) {
            print_mode(current_mode);
        }
        bind_selection_to_mouse(renderer);
        resolve_mode(current_mode, input);

}
