#pragma once

#include "Widget.h"

namespace UI
{
    class TimerDisplay : public Widget
    {
    private:
        const unsigned char& _seconds;
        int _warn_threshold;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;

    public:
        // warn_threshold: seconds at or below which the display turns red
        TimerDisplay(int x, int y, const unsigned char& seconds, int warn_threshold = 10);
    };
}
