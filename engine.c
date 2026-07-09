#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include "engine.h"

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include "engine.h"

SDL_Event sdl_event;

bool engine_running = false;
bool engine_paused = false;

Tick engine_tick_count = 0;

Time engine_time = 0.0;   // simulated engine time in seconds
Time engine_dt = 0.0;     // simulated delta time in seconds

Time engine_overide_dt = 0.0;
bool engine_dt_overwritten = false;

SDLTime sdl_prev_counter = 0;
SDLTime sdl_frequency = 0;

void engine_init() {
    if(engine_running) {
        // Error: engine already running
        return;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    sdl_frequency = SDL_GetPerformanceFrequency();
    sdl_prev_counter = SDL_GetPerformanceCounter();

    engine_time = 0.0;
    engine_dt = 0.0;
    engine_tick_count = 0;

    engine_paused = false;
    engine_running = true;
}

void engine_pause() {
    engine_paused = true;
}
bool engine_is_paused() {
    if(engine_paused) {
        return true;
    }
    return false;
}

void engine_resume() {
    engine_paused = false;
}

void engine_update_time() {
    SDLTime current_counter = SDL_GetPerformanceCounter();

    Time real_dt =
        (Time)(current_counter - sdl_prev_counter) /
        (Time)sdl_frequency;

    sdl_prev_counter = current_counter;

    if(engine_paused || !engine_running) {
        engine_dt = 0.0;
        return;
    }

    if(engine_overide_dt) {
        engine_dt = engine_overide_dt;
    } else {
        engine_dt = real_dt;
    }
    engine_time += engine_dt;
}

void engine_update_tick() {
    if(engine_paused || !engine_running) {
        return;
    }
    engine_tick_count += 1;

}

Tick engine_get_tick() {
    return engine_tick_count;
}

Time engine_get_time() {
    return engine_time;
}

void engine_set_dt(Time dt) {
    engine_overide_dt = dt;
    engine_dt_overwritten = true;
}

void engine_calculate_dt() {
    engine_overide_dt = 0.0;
    engine_dt_overwritten = false;
}

Time engine_get_dt() {
    return engine_dt;
}

void engine_shutdown() {
    engine_running = false;
    SDL_Quit();
}

SDL_Event engine_poll_event() {
    while (SDL_PollEvent(&sdl_event)) {
        return sdl_event;
    }
    return (SDL_Event) {0};
}
