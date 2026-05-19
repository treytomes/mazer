#include "Settings.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

Settings g_settings;

Settings Settings::defaults()
{
    Settings s;
    s.fullscreen   = false;
    s.bgm_volume   = 0.25f;
    s.sfx_volume   = 0.50f;
    s.key_up       = SDL_SCANCODE_UP;
    s.key_down     = SDL_SCANCODE_DOWN;
    s.key_left     = SDL_SCANCODE_LEFT;
    s.key_right    = SDL_SCANCODE_RIGHT;
    s.key_pause    = SDL_SCANCODE_TAB;
    s.key_solution = SDL_SCANCODE_GRAVE;
    return s;
}

static const char* settingsPath()
{
    static char path[512] = {};
    if (path[0] == '\0')
    {
        char* pref = SDL_GetPrefPath("treytomes", "mazer");
        if (pref)
        {
            snprintf(path, sizeof(path), "%ssettings.cfg", pref);
            SDL_free(pref);
        }
        else
        {
            snprintf(path, sizeof(path), "settings.cfg");
        }
    }
    return path;
}

bool Settings::load()
{
    *this = defaults();

    FILE* f = fopen(settingsPath(), "r");
    if (!f) return false;

    char key[64];
    char val[64];
    while (fscanf(f, "%63s %63s", key, val) == 2)
    {
        if      (strcmp(key, "fullscreen")    == 0) fullscreen   = atoi(val) != 0;
        else if (strcmp(key, "bgm_volume")   == 0) bgm_volume   = (float)atof(val);
        else if (strcmp(key, "sfx_volume")   == 0) sfx_volume   = (float)atof(val);
        else if (strcmp(key, "key_up")       == 0) key_up       = (SDL_Scancode)atoi(val);
        else if (strcmp(key, "key_down")     == 0) key_down     = (SDL_Scancode)atoi(val);
        else if (strcmp(key, "key_left")     == 0) key_left     = (SDL_Scancode)atoi(val);
        else if (strcmp(key, "key_right")    == 0) key_right    = (SDL_Scancode)atoi(val);
        else if (strcmp(key, "key_pause")    == 0) key_pause    = (SDL_Scancode)atoi(val);
        else if (strcmp(key, "key_solution") == 0) key_solution = (SDL_Scancode)atoi(val);
    }

    fclose(f);
    return true;
}

void Settings::save() const
{
    FILE* f = fopen(settingsPath(), "w");
    if (!f) return;

    fprintf(f, "fullscreen   %d\n",   fullscreen ? 1 : 0);
    fprintf(f, "bgm_volume   %.3f\n", bgm_volume);
    fprintf(f, "sfx_volume   %.3f\n", sfx_volume);
    fprintf(f, "key_up       %d\n",   (int)key_up);
    fprintf(f, "key_down     %d\n",   (int)key_down);
    fprintf(f, "key_left     %d\n",   (int)key_left);
    fprintf(f, "key_right    %d\n",   (int)key_right);
    fprintf(f, "key_pause    %d\n",   (int)key_pause);
    fprintf(f, "key_solution %d\n",   (int)key_solution);

    fclose(f);
}
