#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H
#include "graphics.h"

/**
 * Initialize the level editor helper state.
 *
 * @return EngineResult describing success or failure.
 */
EngineResult level_editor_init();

/**
 * Process level editor input and update selected entity state.
 *
 * @return EngineResult describing success or failure.
 */
EngineResult level_editor_update();
#endif
