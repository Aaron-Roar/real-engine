#ifndef ENGINE_H
#define ENGINE_H
#include <SDL3/SDL.h>
#include <stdint.h>

typedef Uint64 SDLTime;
typedef double Time;
typedef uint64_t Tick;

void engine_init();
void engine_shutdown();
void engine_update_time();
Time engine_get_time();
Time engine_get_dt();
Tick engine_get_tick();
void engine_pause();
void engine_resume();
void engine_update_tick();
Tick engine_get_tick();
#endif
