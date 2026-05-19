#include "SfxEditor.h"
#include "../KEYBOARD.h"
#include "../AUDIO/SfxPlayer.h"
#include <stdio.h>
#include <string.h>

namespace UI
{
    // Parameter descriptor for display and editing.
    struct ParamDesc
    {
        const char* name;
        float* value;
        float  step;
        float  min_val;
        float  max_val;
        bool   is_int;   // round to nearest integer when editing
    };

    static const int BLINK_SPEED_MS = 400;

    SfxEditor::SfxEditor(int screen_width, int screen_height)
        : Widget(0, 0),
          _screen_width(screen_width), _screen_height(screen_height),
          _preset_index(0), _params(AUDIO::Presets::get(0)),
          _selected_param(0), _scroll_offset(0)
    {
    }

    // Build the parameter table pointing into _params.
    // Called each time we need it (cheap; just pointer assignments).
    static int buildParams(AUDIO::SfxParams& p, ParamDesc out[])
    {
        int n = 0;
        out[n++] = { "wave_type",      (float*)&p.wave_type,     1.0f,  0.0f, 4.0f, true  };
        out[n++] = { "attack_time",    &p.attack_time,            0.05f, 0.0f, 1.0f, false };
        out[n++] = { "sustain_time",   &p.sustain_time,           0.05f, 0.0f, 1.0f, false };
        out[n++] = { "sustain_punch",  &p.sustain_punch,          0.05f, 0.0f, 1.0f, false };
        out[n++] = { "decay_time",     &p.decay_time,             0.05f, 0.0f, 1.0f, false };
        out[n++] = { "base_freq",      &p.base_freq,              0.05f, 0.0f, 1.0f, false };
        out[n++] = { "freq_limit",     &p.freq_limit,             0.05f, 0.0f, 1.0f, false };
        out[n++] = { "freq_ramp",      &p.freq_ramp,              0.05f,-1.0f, 1.0f, false };
        out[n++] = { "freq_dramp",     &p.freq_dramp,             0.05f,-1.0f, 1.0f, false };
        out[n++] = { "vib_strength",   &p.vib_strength,           0.05f, 0.0f, 1.0f, false };
        out[n++] = { "vib_speed",      &p.vib_speed,              0.05f, 0.0f, 1.0f, false };
        out[n++] = { "arp_mod",        &p.arp_mod,                0.05f,-1.0f, 1.0f, false };
        out[n++] = { "arp_speed",      &p.arp_speed,              0.05f, 0.0f, 1.0f, false };
        out[n++] = { "duty",           &p.duty,                   0.05f, 0.0f, 1.0f, false };
        out[n++] = { "duty_ramp",      &p.duty_ramp,              0.05f,-1.0f, 1.0f, false };
        out[n++] = { "repeat_speed",   &p.repeat_speed,           0.05f, 0.0f, 1.0f, false };
        out[n++] = { "lpf_freq",       &p.lpf_freq,               0.05f, 0.0f, 1.0f, false };
        out[n++] = { "lpf_ramp",       &p.lpf_ramp,               0.05f,-1.0f, 1.0f, false };
        out[n++] = { "lpf_resonance",  &p.lpf_resonance,          0.05f, 0.0f, 1.0f, false };
        out[n++] = { "hpf_freq",       &p.hpf_freq,               0.05f, 0.0f, 1.0f, false };
        out[n++] = { "hpf_ramp",       &p.hpf_ramp,               0.05f,-1.0f, 1.0f, false };
        out[n++] = { "volume",         &p.volume,                 0.05f, 0.0f, 1.0f, false };
        return n;
    }

    void SfxEditor::updateSelf(Uint32 /*dt*/)
    {
        ParamDesc params[32];
        int param_count = buildParams(_params, params);

        Uint8 key = KEYBOARD::getScanCode();

        if (key == SDL_SCANCODE_ESCAPE)
        {
            close();
            return;
        }

        // Cycle presets: [ = prev, ] = next
        if (key == SDL_SCANCODE_LEFTBRACKET)
        {
            _preset_index = (_preset_index - 1 + AUDIO::Presets::COUNT) % AUDIO::Presets::COUNT;
            _params = AUDIO::Presets::get(_preset_index);
            _selected_param = 0;
            _scroll_offset  = 0;
            KEYBOARD::setScanCode(0);
            return;
        }
        if (key == SDL_SCANCODE_RIGHTBRACKET)
        {
            _preset_index = (_preset_index + 1) % AUDIO::Presets::COUNT;
            _params = AUDIO::Presets::get(_preset_index);
            _selected_param = 0;
            _scroll_offset  = 0;
            KEYBOARD::setScanCode(0);
            return;
        }

        // Navigate rows
        if (key == SDL_SCANCODE_UP)
        {
            if (_selected_param > 0)
            {
                _selected_param--;
                if (_selected_param < _scroll_offset)
                    _scroll_offset = _selected_param;
            }
            KEYBOARD::setScanCode(0);
            return;
        }
        if (key == SDL_SCANCODE_DOWN)
        {
            if (_selected_param < param_count - 1)
            {
                _selected_param++;
                if (_selected_param >= _scroll_offset + VISIBLE_ROWS)
                    _scroll_offset = _selected_param - VISIBLE_ROWS + 1;
            }
            KEYBOARD::setScanCode(0);
            return;
        }

        // Adjust value
        if (key == SDL_SCANCODE_LEFT || key == SDL_SCANCODE_RIGHT)
        {
            ParamDesc& pd = params[_selected_param];
            float delta = (key == SDL_SCANCODE_RIGHT) ? pd.step : -pd.step;
            float nv = *pd.value + delta;
            if (nv < pd.min_val) nv = pd.min_val;
            if (nv > pd.max_val) nv = pd.max_val;
            if (pd.is_int) nv = (float)(int)(nv + 0.5f);
            *pd.value = nv;
            KEYBOARD::setScanCode(0);
            return;
        }

        // Play preview
        if (key == SDL_SCANCODE_SPACE || key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_KP_ENTER)
        {
            AUDIO::SfxPlayer::play(_params);
            KEYBOARD::setScanCode(0);
            return;
        }
    }

    void SfxEditor::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        int cx = abs_x + _screen_width  / 2;
        int cy = abs_y + 8;  // start near top

        char buf[64];

        // Header
        const char* title = "SFX EDITOR";
        mode->drawString(cx - (int)strlen(title) * 4, cy, 0xA0, title);
        cy += 10;

        // Preset name
        snprintf(buf, sizeof(buf), "[  ] PRESET: %s", AUDIO::Presets::name(_preset_index));
        buf[1] = '<';
        buf[3] = '>';
        mode->drawString(cx - (int)strlen(buf) * 4, cy, 0xF0, buf);
        cy += 10;

        // Column header
        const char* hdr = "PARAMETER        VALUE";
        mode->drawString(cx - (int)strlen(hdr) * 4, cy, 0x80, hdr);
        cy += 10;

        ParamDesc params[32];
        int param_count = buildParams(_params, params);

        int end = _scroll_offset + VISIBLE_ROWS;
        if (end > param_count) end = param_count;

        for (int i = _scroll_offset; i < end; i++)
        {
            ParamDesc& pd = params[i];
            bool sel = (i == _selected_param);
            Uint8 color = sel ? 0xF0 : 0x70;

            char arrow = sel ? '\x10' : ' '; // chr(16) = ►

            if (pd.is_int)
                snprintf(buf, sizeof(buf), "%c %-16s %d", arrow, pd.name, (int)(*pd.value));
            else
                snprintf(buf, sizeof(buf), "%c %-16s %+.2f", arrow, pd.name, *pd.value);

            mode->drawString(cx - (int)strlen(hdr) * 4, cy, color, buf);
            cy += 8;
        }

        // Footer hints
        cy = abs_y + _screen_height - 20;
        const char* hint1 = "UP/DN:select  LT/RT:adjust  SPC:play";
        const char* hint2 = "[ ]:prev preset  ]:next  ESC:back";
        mode->drawString(cx - (int)strlen(hint1) * 4, cy,      0x80, hint1);
        mode->drawString(cx - (int)strlen(hint2) * 4, cy + 8,  0x80, hint2);
    }
}
