#pragma once

#include "Widget.h"
#include "../Settings.h"
#include "../CGA/GraphicsMode.h"

namespace UI
{
    class SettingsScreen : public Widget
    {
    public:
        SettingsScreen(int screen_width, int screen_height,
                       Settings& settings, CGA::GraphicsMode* mode);

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    private:
        void applyVolumes();
        void drawVolumeBar(CGA::GraphicsMode* mode, int x, int y,
                           float value, bool selected) const;

        int                _screen_width;
        int                _screen_height;
        Settings&          _settings;
        CGA::GraphicsMode* _mode;

        // Rows: 0=fullscreen, 1=BGM vol, 2=SFX vol, 3=key_up, 4=key_down,
        //       5=key_left, 6=key_right, 7=key_pause, 8=key_solution
        static const int ROW_COUNT = 9;
        int  _selected;

        // When >= 0, we're waiting for a key press to rebind this row.
        int  _rebinding; // -1 = not rebinding

        // Brief flash message after saving.
        int  _saved_flash_ms;
    };
}
