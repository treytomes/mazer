#pragma once

#include <SDL2/SDL.h>

struct Settings
{
    // Volume levels (0.0–1.0).
    float bgm_volume;
    float sfx_volume;

    // Key bindings (SDL scancodes).
    SDL_Scancode key_up;
    SDL_Scancode key_down;
    SDL_Scancode key_left;
    SDL_Scancode key_right;
    SDL_Scancode key_pause;
    SDL_Scancode key_solution; // hold to show maze solution

    static Settings defaults();
    bool load();   // returns false if no saved file; populates defaults
    void save() const;
};

// Global instance — populated at startup before any game loop runs.
extern Settings g_settings;
