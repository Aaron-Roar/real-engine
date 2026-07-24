#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <SDL3/SDL.h>
#include "math2d.h"

/** Engine keyboard key identifiers. */
typedef enum KeyboardKey {
    /** No key. */
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

/** Per-key state tracked across frames. */
typedef enum {
    /** Key is not held. */
    KEY_STATE_UP = 0,
    /** Key is held. */
    KEY_STATE_DOWN,
    /** Key was pressed this frame. */
    KEY_STATE_PRESSED,
    /** Key was released this frame. */
    KEY_STATE_RELEASED,
    /** No valid key state. */
    KEY_STATE_NONE,
} KeyboardKeyState;

/** Current state table for all keyboard keys. */
typedef struct {
    /** Key states indexed by KeyboardKey. */
    KeyboardKeyState key_states[KEY_COUNT];
} KeyboardState;

/** One keyboard input event. */
typedef struct {
    /** Key involved in the event. */
    KeyboardKey key;
    /** New key state from the event. */
    KeyboardKeyState state;
} KeyboardEvent;

/** Engine mouse button identifiers. */
typedef enum {
    /** No mouse button. */
    MOUSE_BUTTON_NONE = 0,
    /** Left mouse button. */
    MOUSE_BUTTON_LEFT,
    /** Right mouse button. */
    MOUSE_BUTTON_RIGHT,
    /** Middle mouse button. */
    MOUSE_BUTTON_MIDDLE,
    /** Number of mouse button entries. */
    MOUSE_BUTTON_COUNT,
} MouseButton;

/** Per-button mouse state tracked across frames. */
typedef enum {
    /** Button is not held. */
    MOUSE_BUTTON_STATE_UP = 0,
    /** Button is held. */
    MOUSE_BUTTON_STATE_DOWN,
    /** Button was pressed this frame. */
    MOUSE_BUTTON_STATE_PRESSED,
    /** Button was released this frame. */
    MOUSE_BUTTON_STATE_RELEASED,
    /** No valid button state. */
    MOUSE_BUTTON_STATE_NONE,
} MouseButtonState;

/** Mouse position in world coordinates. */
typedef Vec2D MousePosition;

/** Current mouse state. */
typedef struct {
    /** Button states indexed by MouseButton. */
    MouseButtonState button_states[MOUSE_BUTTON_COUNT];
    /** Current mouse position. */
    MousePosition position;
} MouseState;

/** One mouse input event. */
typedef struct {
    /** Button involved in the event. */
    MouseButton button;
    /** New button state from the event. */
    MouseButtonState state;
    /** Mouse position at event capture time. */
    MousePosition position;
} MouseEvent;


/** Print a keyboard event to the console. */
void print_keyboard_event(KeyboardEvent event);

/** Advance transient key states to held/up states. */
void update_key_states(KeyboardState *keyboard);

/** Apply one keyboard event to keyboard state. */
void add_key_event(KeyboardState *keyboard, KeyboardEvent key_event);

/** Convert an SDL event to an engine keyboard event. */
KeyboardEvent capture_keyboard_event(const SDL_Event *sdl_event);

/** Print a mouse event to the console. */
void print_mouse_event(MouseEvent event);

/** Advance transient mouse button states to held/up states. */
void update_mouse_states(MouseState *mouse);

/** Apply one mouse event to mouse state. */
void add_mouse_event(MouseState *mouse, MouseEvent mouse_event);

/** Convert an SDL event to an engine mouse event. */
MouseEvent capture_mouse_event(const SDL_Event *sdl_event);
#endif
