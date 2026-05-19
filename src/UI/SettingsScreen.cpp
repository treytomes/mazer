#include "SettingsScreen.h"
#include "../KEYBOARD.h"
#include "../AUDIO/AudioSystem.h"
#include "../AUDIO/SfxPlayer.h"
#include "../AUDIO/SfxParams.h"
#include "../SYSTEM/SYSTEM.h"
#include <stdio.h>
#include <string.h>

namespace UI
{
    enum RowType { ROW_VOLUME, ROW_BINDING, ROW_TOGGLE };

    struct RowDesc
    {
        const char*   label;
        RowType       type;
        SDL_Scancode* binding;  // ROW_BINDING
        float*        volume;   // ROW_VOLUME
        bool*         toggle;   // ROW_TOGGLE
    };

    static const float VOL_STEP  = 0.05f;
    static const int   BAR_WIDTH = 20; // characters

    SettingsScreen::SettingsScreen(int sw, int sh, Settings& settings)
        : Widget(0, 0),
          _screen_width(sw), _screen_height(sh),
          _settings(settings),
          _selected(0), _rebinding(-1), _saved_flash_ms(0)
    {
    }

    void SettingsScreen::applyVolumes()
    {
        AUDIO::AudioSystem::instance().setVolume(AUDIO::CHANNEL_BGM, _settings.bgm_volume);
        AUDIO::AudioSystem::instance().setVolume(AUDIO::CHANNEL_SFX, _settings.sfx_volume);
    }

    static void buildRows(Settings& s, RowDesc rows[])
    {
        rows[0] = { "Fullscreen",    ROW_TOGGLE,  nullptr,         nullptr,        &s.fullscreen  };
        rows[1] = { "BGM Volume",    ROW_VOLUME,  nullptr,         &s.bgm_volume,  nullptr        };
        rows[2] = { "SFX Volume",    ROW_VOLUME,  nullptr,         &s.sfx_volume,  nullptr        };
        rows[3] = { "Move Up",       ROW_BINDING, &s.key_up,       nullptr,        nullptr        };
        rows[4] = { "Move Down",     ROW_BINDING, &s.key_down,     nullptr,        nullptr        };
        rows[5] = { "Move Left",     ROW_BINDING, &s.key_left,     nullptr,        nullptr        };
        rows[6] = { "Move Right",    ROW_BINDING, &s.key_right,    nullptr,        nullptr        };
        rows[7] = { "Pause",         ROW_BINDING, &s.key_pause,    nullptr,        nullptr        };
        rows[8] = { "Show Solution", ROW_BINDING, &s.key_solution, nullptr,        nullptr        };
    }

    void SettingsScreen::updateSelf(Uint32 dt)
    {
        if (_saved_flash_ms > 0)
            _saved_flash_ms -= (int)dt;

        RowDesc rows[ROW_COUNT];
        buildRows(_settings, rows);

        // Rebind mode: swallow the next non-modifier keypress.
        if (_rebinding >= 0)
        {
            Uint8 sc = KEYBOARD::getScanCode();
            if (sc != 0
                && sc != SDL_SCANCODE_LSHIFT  && sc != SDL_SCANCODE_RSHIFT
                && sc != SDL_SCANCODE_LCTRL   && sc != SDL_SCANCODE_RCTRL
                && sc != SDL_SCANCODE_LALT    && sc != SDL_SCANCODE_RALT
                && sc != SDL_SCANCODE_LGUI    && sc != SDL_SCANCODE_RGUI)
            {
                if (sc != SDL_SCANCODE_ESCAPE)
                    *rows[_rebinding].binding = (SDL_Scancode)sc;
                _rebinding = -1;
                KEYBOARD::setScanCode(0);
            }
            return;
        }

        Uint8 key = KEYBOARD::getScanCode();
        if (key == 0) return;

        if (key == SDL_SCANCODE_ESCAPE)
        {
            _settings.save();
            AUDIO::SfxPlayer::play(AUDIO::Presets::MenuSelect());
            close();
            KEYBOARD::setScanCode(0);
            return;
        }

        if (key == SDL_SCANCODE_UP)
        {
            if (_selected > 0) _selected--;
            AUDIO::SfxPlayer::play(AUDIO::Presets::MenuNavigate());
            KEYBOARD::setScanCode(0);
            return;
        }
        if (key == SDL_SCANCODE_DOWN)
        {
            if (_selected < ROW_COUNT - 1) _selected++;
            AUDIO::SfxPlayer::play(AUDIO::Presets::MenuNavigate());
            KEYBOARD::setScanCode(0);
            return;
        }

        RowDesc& row = rows[_selected];

        if (row.type == ROW_VOLUME)
        {
            float delta = 0.0f;
            if (key == SDL_SCANCODE_LEFT)  delta = -VOL_STEP;
            if (key == SDL_SCANCODE_RIGHT) delta =  VOL_STEP;
            if (delta != 0.0f)
            {
                float nv = *row.volume + delta;
                if (nv < 0.0f) nv = 0.0f;
                if (nv > 1.0f) nv = 1.0f;
                // Round to nearest step to avoid float drift.
                nv = (float)(int)(nv / VOL_STEP + 0.5f) * VOL_STEP;
                *row.volume = nv;
                applyVolumes();
                AUDIO::SfxPlayer::play(AUDIO::Presets::MenuNavigate());
                KEYBOARD::setScanCode(0);
            }
        }
        else if (row.type == ROW_TOGGLE)
        {
            bool activate = (key == SDL_SCANCODE_LEFT  ||
                             key == SDL_SCANCODE_RIGHT ||
                             key == SDL_SCANCODE_RETURN ||
                             key == SDL_SCANCODE_KP_ENTER);
            if (activate)
            {
                *row.toggle = !*row.toggle;
                SYSTEM::toggleFullscreen();
                AUDIO::SfxPlayer::play(AUDIO::Presets::MenuNavigate());
                KEYBOARD::setScanCode(0);
            }
        }
        else // ROW_BINDING
        {
            if (key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_KP_ENTER)
            {
                _rebinding = _selected;
                KEYBOARD::setScanCode(0);
            }
        }

        // S or Enter on the last action = save explicitly (also saves on Escape).
        if (key == SDL_SCANCODE_S)
        {
            _settings.save();
            applyVolumes();
            _saved_flash_ms = 1500;
            AUDIO::SfxPlayer::play(AUDIO::Presets::MenuSelect());
            KEYBOARD::setScanCode(0);
        }
    }

    void SettingsScreen::drawVolumeBar(CGA::GraphicsMode* mode, int x, int y,
                                       float value, bool selected) const
    {
        char bar[BAR_WIDTH + 3]; // [ + chars + ]
        bar[0] = '[';
        int filled = (int)(value * BAR_WIDTH + 0.5f);
        for (int i = 0; i < BAR_WIDTH; i++)
            bar[1 + i] = (i < filled) ? '\xDB' : ' '; // chr(219) full block
        bar[BAR_WIDTH + 1] = ']';
        bar[BAR_WIDTH + 2] = '\0';

        char pct[8];
        snprintf(pct, sizeof(pct), " %3d%%", (int)(value * 100.0f + 0.5f));

        Uint8 color = selected ? 0xF0 : 0x70;
        mode->drawString(x, y, color, bar);
        mode->drawString(x + (BAR_WIDTH + 2) * 8, y, color, pct);
    }

    void SettingsScreen::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        const int cx  = abs_x + _screen_width / 2;
        const int top = abs_y + 12;
        int y = top;

        const char* title = "SETTINGS";
        mode->drawString(cx - (int)strlen(title) * 4, y, 0xA0, title);
        y += 12;

        const char* hint_vol = "LT/RT: adjust  ESC: save & back  S: save";
        mode->drawString(cx - (int)strlen(hint_vol) * 4, y, 0x80, hint_vol);
        y += 12;

        RowDesc rows[ROW_COUNT];
        buildRows(_settings, rows);

        for (int i = 0; i < ROW_COUNT; i++)
        {
            bool sel  = (i == _selected);
            Uint8 col = sel ? 0xF0 : 0x70;

            char prefix[3] = "  ";
            if (sel) prefix[0] = '\x10'; // ►

            // Left column: label
            char lbuf[32];
            snprintf(lbuf, sizeof(lbuf), "%s %-14s", prefix, rows[i].label);
            mode->drawString(abs_x + 8, y, col, lbuf);

            // Right column: value
            if (rows[i].type == ROW_VOLUME)
            {
                drawVolumeBar(mode, abs_x + 8 + 18 * 8, y, *rows[i].volume, sel);
            }
            else if (rows[i].type == ROW_TOGGLE)
            {
                const char* val = *rows[i].toggle ? "ON" : "OFF";
                mode->drawString(abs_x + 8 + 18 * 8, y, col, val);
            }
            else // ROW_BINDING
            {
                const char* name;
                if (_rebinding == i)
                    name = "< PRESS KEY >";
                else
                    name = SDL_GetScancodeName(*rows[i].binding);

                Uint8 vc = (_rebinding == i) ? 0xE0 : col;
                mode->drawString(abs_x + 8 + 18 * 8, y, vc, name);
            }

            y += 10;

            // Section separators.
            if (i == 0 || i == 2)
                y += 4;
        }

        if (_saved_flash_ms > 0)
        {
            const char* saved = "SAVED!";
            mode->drawString(cx - (int)strlen(saved) * 4,
                             abs_y + _screen_height - 16, 0xA0, saved);
        }
    }
}
