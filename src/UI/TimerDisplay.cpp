#include "TimerDisplay.h"
#include <stdio.h>

namespace UI
{
    TimerDisplay::TimerDisplay(int x, int y, const unsigned char& seconds, int warn_threshold)
        : Widget(x, y), _seconds(seconds), _warn_threshold(warn_threshold)
    {
    }

    void TimerDisplay::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        char buffer[12];
        sprintf(buffer, "TIME:%03d", _seconds);
        Uint8 color = (_seconds <= _warn_threshold) ? 0xF4 : 0xE0;
        mode->drawString(abs_x, abs_y, color, buffer);
    }
}
