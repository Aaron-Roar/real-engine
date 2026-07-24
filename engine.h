#ifndef ENGINE_H
#define ENGINE_H
#include <SDL3/SDL.h>
#include <stdint.h>
#include "error.h"

typedef Uint64 SDLTime;
typedef double Time;
typedef uint64_t Tick;

EngineResult engine_init();
void engine_shutdown();
void engine_update_time();
Time engine_get_time();
Tick engine_get_tick();
void engine_pause();
void engine_resume();
void engine_update_tick();
Tick engine_get_tick();
SDL_Event engine_poll_event();
bool engine_is_paused();
Time engine_get_dt();
void engine_calculate_dt();
void engine_set_dt(Time dt);
void engine_reset_clock();
#endif
