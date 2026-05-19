#pragma once

#include "Widget.h"

namespace UI
{
    enum class MainMenuResult { None, Play, SFX, Settings, Quit };

    class MainMenu : public Widget
    {
    private:
        int _screen_width;
        int _screen_height;
        int _selected;          // 0 = Play, 1 = Quit
        MainMenuResult _result;

        Uint32 _blink_timer;
        bool   _blink_visible;

        int    _exit_delay_ms;  // > 0 while waiting before close() after Quit

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    public:
        MainMenu(int screen_width, int screen_height);

        // Valid after wantsClose() returns true.
        MainMenuResult result() const;
    };
}
