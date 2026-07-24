#include "controller.h"
#include "console.h"
#include "graphics.h"

static const KeyboardKey SDL_TO_ENGINE_KEY[SDL_SCANCODE_COUNT] = {
    [SDL_SCANCODE_0] = KEY_0,
    [SDL_SCANCODE_1] = KEY_1,
    [SDL_SCANCODE_2] = KEY_2,
    [SDL_SCANCODE_3] = KEY_3,
    [SDL_SCANCODE_4] = KEY_4,
    [SDL_SCANCODE_5] = KEY_5,
    [SDL_SCANCODE_6] = KEY_6,
    [SDL_SCANCODE_7] = KEY_7,
    [SDL_SCANCODE_8] = KEY_8,
    [SDL_SCANCODE_9] = KEY_9,

    [SDL_SCANCODE_A] = KEY_A,
    [SDL_SCANCODE_B] = KEY_B,
    [SDL_SCANCODE_C] = KEY_C,
    [SDL_SCANCODE_D] = KEY_D,
    [SDL_SCANCODE_E] = KEY_E,
    [SDL_SCANCODE_F] = KEY_F,
    [SDL_SCANCODE_G] = KEY_G,
    [SDL_SCANCODE_H] = KEY_H,
    [SDL_SCANCODE_I] = KEY_I,
    [SDL_SCANCODE_J] = KEY_J,
    [SDL_SCANCODE_K] = KEY_K,
    [SDL_SCANCODE_L] = KEY_L,
    [SDL_SCANCODE_M] = KEY_M,
    [SDL_SCANCODE_N] = KEY_N,
    [SDL_SCANCODE_O] = KEY_O,
    [SDL_SCANCODE_P] = KEY_P,
    [SDL_SCANCODE_Q] = KEY_Q,
    [SDL_SCANCODE_R] = KEY_R,
    [SDL_SCANCODE_S] = KEY_S,
    [SDL_SCANCODE_T] = KEY_T,
    [SDL_SCANCODE_U] = KEY_U,
    [SDL_SCANCODE_V] = KEY_V,
    [SDL_SCANCODE_W] = KEY_W,
    [SDL_SCANCODE_X] = KEY_X,
    [SDL_SCANCODE_Y] = KEY_Y,
    [SDL_SCANCODE_Z] = KEY_Z,

    [SDL_SCANCODE_SPACE]     = KEY_SPACE,
    [SDL_SCANCODE_RETURN]    = KEY_ENTER,
    [SDL_SCANCODE_ESCAPE]    = KEY_ESCAPE,
    [SDL_SCANCODE_BACKSPACE] = KEY_BACKSPACE,
    [SDL_SCANCODE_DELETE]    = KEY_DELETE,
    [SDL_SCANCODE_EQUALS]    = KEY_EQUALS,
    [SDL_SCANCODE_MINUS]     = KEY_MINUS,

    [SDL_SCANCODE_LSHIFT] = KEY_SHIFT,
    [SDL_SCANCODE_RSHIFT] = KEY_SHIFT,
    [SDL_SCANCODE_LALT]   = KEY_ALT,
    [SDL_SCANCODE_RALT]   = KEY_ALT,
    [SDL_SCANCODE_LCTRL]  = KEY_CTRL,
    [SDL_SCANCODE_RCTRL]  = KEY_CTRL,

    [SDL_SCANCODE_UP]    = KEY_UP,
    [SDL_SCANCODE_DOWN]  = KEY_DOWN,
    [SDL_SCANCODE_LEFT]  = KEY_LEFT,
    [SDL_SCANCODE_RIGHT] = KEY_RIGHT,

    [SDL_SCANCODE_TAB] = KEY_TAB,
};


KeyboardEvent capture_keyboard_event(const SDL_Event *sdl_event) {
    KeyboardEvent key_event = {
        .key = KEY_NONE,
        .state = KEY_STATE_NONE,
    };

    if (sdl_event == NULL) {
        return key_event;
    }
    if (sdl_event->type != SDL_EVENT_KEY_DOWN &&
        sdl_event->type != SDL_EVENT_KEY_UP) {
        return key_event;
    }

    SDL_Scancode scancode = sdl_event->key.scancode;
    if (scancode < 0 || scancode >= SDL_SCANCODE_COUNT) {
        return key_event;
    }
    KeyboardKey key = SDL_TO_ENGINE_KEY[scancode];
    if(key == KEY_NONE) {
        return key_event;
    }
    key_event.key = key;

    if(sdl_event->type == SDL_EVENT_KEY_DOWN) {
        if(sdl_event->key.repeat) {
            return key_event;
        }
        key_event.state = KEY_STATE_PRESSED;
    }
    else if(sdl_event->type == SDL_EVENT_KEY_UP) {
        key_event.state = KEY_STATE_RELEASED;
    }

    return key_event;
}

void add_key_event(KeyboardState *keyboard, KeyboardEvent key_event) {
    if(keyboard == NULL) {
        return;
    }
    if(
        key_event.key <= KEY_NONE ||
        key_event.state == KEY_STATE_NONE ||
        key_event.key >= KEY_COUNT) {
        return;
    }

    KeyboardKeyState current_key_state = keyboard->key_states[key_event.key];
    if(key_event.state == KEY_STATE_PRESSED) {
        if(current_key_state == KEY_STATE_DOWN || current_key_state == KEY_STATE_PRESSED) {
            return;
        }
        keyboard->key_states[key_event.key] = KEY_STATE_PRESSED;
    }
    if(key_event.state == KEY_STATE_RELEASED) {
        if(current_key_state == KEY_STATE_UP || current_key_state == KEY_STATE_RELEASED) {
            return;
        }
        keyboard->key_states[key_event.key] = KEY_STATE_RELEASED;
    }
}

void update_key_states(KeyboardState *keyboard) {
    if(keyboard == NULL) {
        return;
    }

    for(int i = 0; i < KEY_COUNT; i += 1) {
        if(keyboard->key_states[i] == KEY_STATE_RELEASED) {
            keyboard->key_states[i] = KEY_STATE_UP;
        }
        else if(keyboard->key_states[i] == KEY_STATE_PRESSED) {
            keyboard->key_states[i] = KEY_STATE_DOWN;
        }
    }
}

#include <stdio.h>

const char *keyboard_key_name(KeyboardKey key)
{
    static const char *names[KEY_COUNT] = {
        [KEY_NONE]      = "NONE",

        [KEY_0]         = "0",
        [KEY_1]         = "1",
        [KEY_2]         = "2",
        [KEY_3]         = "3",
        [KEY_4]         = "4",
        [KEY_5]         = "5",
        [KEY_6]         = "6",
        [KEY_7]         = "7",
        [KEY_8]         = "8",
        [KEY_9]         = "9",

        [KEY_A]         = "A",
        [KEY_B]         = "B",
        [KEY_C]         = "C",
        [KEY_D]         = "D",
        [KEY_E]         = "E",
        [KEY_F]         = "F",
        [KEY_G]         = "G",
        [KEY_H]         = "H",
        [KEY_I]         = "I",
        [KEY_J]         = "J",
        [KEY_K]         = "K",
        [KEY_L]         = "L",
        [KEY_M]         = "M",
        [KEY_N]         = "N",
        [KEY_O]         = "O",
        [KEY_P]         = "P",
        [KEY_Q]         = "Q",
        [KEY_R]         = "R",
        [KEY_S]         = "S",
        [KEY_T]         = "T",
        [KEY_U]         = "U",
        [KEY_V]         = "V",
        [KEY_W]         = "W",
        [KEY_X]         = "X",
        [KEY_Y]         = "Y",
        [KEY_Z]         = "Z",

        [KEY_SPACE]     = "SPACE",
        [KEY_ENTER]     = "ENTER",
        [KEY_ESCAPE]    = "ESCAPE",
        [KEY_BACKSPACE] = "BACKSPACE",
        [KEY_DELETE]    = "DELETE",
        [KEY_PLUS]      = "PLUS",
        [KEY_MINUS]     = "MINUS",
        [KEY_EQUALS]    = "EQUALS",
        [KEY_SHIFT]     = "SHIFT",
        [KEY_ALT]       = "ALT",
        [KEY_CTRL]      = "CTRL",
        [KEY_UP]        = "UP",
        [KEY_DOWN]      = "DOWN",
        [KEY_LEFT]      = "LEFT",
        [KEY_RIGHT]     = "RIGHT",
        [KEY_TAB]       = "TAB",
    };

    if (key < KEY_NONE || key >= KEY_COUNT) {
        return "INVALID_KEY";
    }

    return names[key] != NULL ? names[key] : "UNNAMED_KEY";
}

const char *keyboard_state_name(KeyboardKeyState state)
{
    switch (state) {
        case KEY_STATE_NONE:
            return "NONE";

        case KEY_STATE_UP:
            return "UP";

        case KEY_STATE_DOWN:
            return "DOWN";

        case KEY_STATE_PRESSED:
            return "PRESSED";

        case KEY_STATE_RELEASED:
            return "RELEASED";

        default:
            return "INVALID_STATE";
    }
}

void print_keyboard_event(KeyboardEvent event)
{
    if(event.key == KEY_NONE || event.state == KEY_STATE_NONE) {
        return;
    }
    console_write(LOG_ENGINE,
        "KeyboardEvent { key: %s, state: %s }\n",
        keyboard_key_name(event.key),
        keyboard_state_name(event.state)
    );
}

Vec2D get_mouse_position(void) {
    Position world_position =
        graphics_screen_to_world(graphics_get_mouse_screen_position());

    return (Vec2D){
        .x = world_position.x,
        .y = world_position.y,
    };
}
MouseEvent capture_mouse_event(const SDL_Event *sdl_event) {
    MouseEvent mouse_event = {
        .button = MOUSE_BUTTON_NONE,
        .state = MOUSE_BUTTON_STATE_NONE,
        .position = {0,0},
    };

    if (sdl_event == NULL) {
        return mouse_event;
    }
    if (sdl_event->type != SDL_EVENT_MOUSE_BUTTON_DOWN &&
        sdl_event->type != SDL_EVENT_MOUSE_BUTTON_UP) {
        return mouse_event;
    }

    if(sdl_event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        switch(sdl_event->button.button) {
            case SDL_BUTTON_LEFT:
                mouse_event.button = MOUSE_BUTTON_LEFT;
                mouse_event.state = MOUSE_BUTTON_STATE_PRESSED;
                break;
            case SDL_BUTTON_RIGHT:
                mouse_event.button = MOUSE_BUTTON_RIGHT;
                mouse_event.state = MOUSE_BUTTON_STATE_PRESSED;
                break;
            case SDL_BUTTON_MIDDLE:
                mouse_event.button = MOUSE_BUTTON_MIDDLE;
                mouse_event.state = MOUSE_BUTTON_STATE_PRESSED;
                break;
            default:
                break;
        }

    }
    else if(sdl_event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        switch(sdl_event->button.button) {
            case SDL_BUTTON_LEFT:
                mouse_event.button = MOUSE_BUTTON_LEFT;
                mouse_event.state = MOUSE_BUTTON_STATE_RELEASED;
                break;
            case SDL_BUTTON_RIGHT:
                mouse_event.button = MOUSE_BUTTON_RIGHT;
                mouse_event.state = MOUSE_BUTTON_STATE_RELEASED;
                break;
            case SDL_BUTTON_MIDDLE:
                mouse_event.button = MOUSE_BUTTON_MIDDLE;
                mouse_event.state = MOUSE_BUTTON_STATE_RELEASED;
                break;
            default:
                break;
        }
    }
    mouse_event.position = get_mouse_position();

    return mouse_event;
}

void add_mouse_event(MouseState *mouse, MouseEvent mouse_event) {
    if(mouse == NULL) {
        return;
    }
    if(
        mouse_event.button <= MOUSE_BUTTON_NONE ||
        mouse_event.state == MOUSE_BUTTON_STATE_NONE ||
        mouse_event.button >= MOUSE_BUTTON_COUNT) {
        return;
    }

    MouseButtonState current_mouse_button_state = mouse->button_states[mouse_event.button];
    if(mouse_event.state == MOUSE_BUTTON_STATE_PRESSED) {
        if(current_mouse_button_state == MOUSE_BUTTON_STATE_DOWN || current_mouse_button_state == MOUSE_BUTTON_STATE_PRESSED) {
            return;
        }
        mouse->button_states[mouse_event.button] = MOUSE_BUTTON_STATE_PRESSED;
    }
    if(mouse_event.state == MOUSE_BUTTON_STATE_RELEASED) {
        if(current_mouse_button_state == MOUSE_BUTTON_STATE_UP || current_mouse_button_state == MOUSE_BUTTON_STATE_RELEASED) {
            return;
        }
        mouse->button_states[mouse_event.button] = MOUSE_BUTTON_STATE_RELEASED;
    }
    mouse->position = mouse_event.position;
}

void update_mouse_states(MouseState *mouse) {
    if(mouse == NULL) {
        return;
    }

    for(int i = 0; i < MOUSE_BUTTON_COUNT; i += 1) {
        if(mouse->button_states[i] == MOUSE_BUTTON_STATE_RELEASED) {
            mouse->button_states[i] = MOUSE_BUTTON_STATE_UP;
        }
        else if(mouse->button_states[i] == MOUSE_BUTTON_STATE_PRESSED) {
            mouse->button_states[i] = MOUSE_BUTTON_STATE_DOWN;
        }
    }
    mouse->position = get_mouse_position();
}
const char *mouse_button_name(MouseButton button)
{
    switch (button) {
        case MOUSE_BUTTON_NONE:
            return "NONE";

        case MOUSE_BUTTON_LEFT:
            return "LEFT";

        case MOUSE_BUTTON_MIDDLE:
            return "MIDDLE";

        case MOUSE_BUTTON_RIGHT:
            return "RIGHT";

        default:
            return "INVALID_BUTTON";
    }
}

const char *mouse_button_state_name(MouseButtonState state)
{
    switch (state) {
        case MOUSE_BUTTON_STATE_NONE:
            return "NONE";

        case MOUSE_BUTTON_STATE_UP:
            return "UP";

        case MOUSE_BUTTON_STATE_DOWN:
            return "DOWN";

        case MOUSE_BUTTON_STATE_PRESSED:
            return "PRESSED";

        case MOUSE_BUTTON_STATE_RELEASED:
            return "RELEASED";

        default:
            return "INVALID_STATE";
    }
}

void print_mouse_event(MouseEvent event)
{
    if (event.button == MOUSE_BUTTON_NONE ||
        event.state == MOUSE_BUTTON_STATE_NONE) {
        return;
    }

    console_write(
        LOG_ENGINE,
        "MouseEvent { button: %s, state: %s, position: { x: %.2f, y: %.2f } }\n",
        mouse_button_name(event.button),
        mouse_button_state_name(event.state),
        event.position.x,
        event.position.y
    );
}
