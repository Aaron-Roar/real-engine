#ifndef ENGINE_H
#define ENGINE_H
#include <SDL3/SDL.h>
#include <stdint.h>
#include "error.h"

/** SDL performance counter value used for engine timing. */
typedef Uint64 SDLTime;

/** Engine time value, measured in seconds. */
typedef double Time;

/** Monotonic engine tick counter. */
typedef uint64_t Tick;

/**
 * Initialize SDL and all engine-owned subsystem tables.
 *
 * @return EngineResult containing true on success, or an error describing the
 * failing subsystem.
 */
EngineResult engine_init();

/**
 * Shut down all engine subsystems and SDL.
 */
void engine_shutdown();

/**
 * Update engine time and delta time from SDL's performance counter.
 *
 * If the engine is paused, delta time is set to zero.
 */
void engine_update_time();

/**
 * Get accumulated engine time in seconds.
 *
 * @return Current simulated engine time.
 */
Time engine_get_time();

/**
 * Get the current engine tick count.
 *
 * @return Number of ticks advanced since initialization or reset.
 */
Tick engine_get_tick();

/**
 * Pause engine time and tick advancement.
 */
void engine_pause();

/**
 * Resume engine time and tick advancement.
 */
void engine_resume();

/**
 * Advance the engine tick count by one when the engine is running.
 */
void engine_update_tick();

/**
 * Poll one SDL event.
 *
 * @return The next SDL event, or a zeroed SDL_Event when no event is pending.
 */
SDL_Event engine_poll_event();

/**
 * Check whether the engine is paused.
 *
 * @return true when paused, false otherwise.
 */
bool engine_is_paused();

/**
 * Get the current engine delta time.
 *
 * @return Delta time in seconds from the last engine_update_time() call.
 */
Time engine_get_dt();

/**
 * Return delta time calculation to real elapsed time.
 */
void engine_calculate_dt();

/**
 * Override the engine delta time with a fixed value.
 *
 * @param dt Fixed delta time in seconds.
 */
void engine_set_dt(Time dt);

/**
 * Reset the internal timing baseline without advancing time.
 */
void engine_reset_clock();
#endif
