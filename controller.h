#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <SDL3/SDL.h>
#include "math2d.h"
typedef enum KeyboardKey {
    KEY_NONE = 0,

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
    KEY_SPACE,

    KEY_ENTER,
    KEY_ESCAPE,

    KEY_BACKSPACE,
    KEY_DELETE,

    KEY_PLUS,
    KEY_MINUS,
    KEY_EQUALS,

    KEY_SHIFT,
    KEY_ALT,
    KEY_CTRL,

    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    KEY_TAB,

    KEY_COUNT
} KeyboardKey;
typedef enum {
    KEY_STATE_UP = 0,
    KEY_STATE_DOWN,
    KEY_STATE_PRESSED,
    KEY_STATE_RELEASED,
    KEY_STATE_NONE,
} KeyboardKeyState;
typedef struct {
    KeyboardKeyState key_states[KEY_COUNT];
} KeyboardState;
typedef struct {
    KeyboardKey key;
    KeyboardKeyState state;
} KeyboardEvent;

typedef enum {
    MOUSE_BUTTON_NONE = 0,
    MOUSE_BUTTON_LEFT,
    MOUSE_BUTTON_RIGHT,
    MOUSE_BUTTON_MIDDLE,
    MOUSE_BUTTON_COUNT,
} MouseButton;
typedef enum {
    MOUSE_BUTTON_STATE_UP = 0,
    MOUSE_BUTTON_STATE_DOWN,
    MOUSE_BUTTON_STATE_PRESSED,
    MOUSE_BUTTON_STATE_RELEASED,
    MOUSE_BUTTON_STATE_NONE,
} MouseButtonState;
typedef Vec2D MousePosition;
typedef struct {
    MouseButtonState button_states[MOUSE_BUTTON_COUNT];
    MousePosition position;
} MouseState;
typedef struct {
    MouseButton button;
    MouseButtonState state;
    MousePosition position;
} MouseEvent;



void print_keyboard_event(KeyboardEvent event);
void update_key_states(KeyboardState *keyboard);
void add_key_event(KeyboardState *keyboard, KeyboardEvent key_event);
KeyboardEvent capture_keyboard_event(const SDL_Event *sdl_event);

void print_mouse_event(MouseEvent event);
void update_mouse_states(MouseState *mouse);
void add_mouse_event(MouseState *mouse, MouseEvent mouse_event);
MouseEvent capture_mouse_event(const SDL_Event *sdl_event);
#endif
