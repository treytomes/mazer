#pragma once

#include "Widget.h"
#include "../Settings.h"

namespace UI
{
    class SettingsScreen : public Widget
    {
    public:
        SettingsScreen(int screen_width, int screen_height, Settings& settings);

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    private:
        void applyVolumes();
        void drawVolumeBar(CGA::GraphicsMode* mode, int x, int y,
                           float value, bool selected) const;

        int       _screen_width;
        int       _screen_height;
        Settings& _settings;

        // Rows: 0=BGM vol, 1=SFX vol, 2=key_up, 3=key_down,
        //       4=key_left, 5=key_right, 6=key_pause, 7=key_solution
        static const int ROW_COUNT = 8;
        int  _selected;

        // When >= 0, we're waiting for a key press to rebind this row.
        int  _rebinding; // -1 = not rebinding

        // Brief flash message after saving.
        int  _saved_flash_ms;
    };
}
