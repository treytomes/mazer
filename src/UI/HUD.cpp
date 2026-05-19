#include "HUD.h"
#include "HeartMeter.h"
#include "TimerDisplay.h"
#include "BlockCounter.h"
#include <stdio.h>
#include <string.h>

namespace UI
{
    // Character width in pixels used for layout calculations.
    static const int CHAR_W = 8;

    HUD::HUD(int screen_width,
             const int& health,
             const int& score,
             const int& block_count,
             const unsigned char& seconds)
        : Widget(0, 0),
          _health(health), _score(score),
          _block_count(block_count), _seconds(seconds),
          _screen_width(screen_width)
    {
    }

    void HUD::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        // Clear HUD background strip.
        mode->drawFilledRect(abs_x, abs_x + _screen_width - 1, abs_y, abs_y + 7, 0);

        // Right-align: time, then block counter, then score, working left.
        int x = _screen_width;

        char buffer[32];

        // Time
        sprintf(buffer, "TIME:%03d", _seconds);
        x -= (int)strlen(buffer) * CHAR_W;
        Uint8 time_color = (_seconds <= 10) ? 0xF4 : 0xE0;
        mode->drawString(abs_x + x, abs_y, time_color, buffer);

        // Block counter
        sprintf(buffer, "x%d", _block_count);
        x -= (int)strlen(buffer) * CHAR_W + CHAR_W;
        mode->drawString(abs_x + x, abs_y, 0xE0, buffer);
        x -= CHAR_W;
        mode->drawChar(abs_x + x, abs_y, 0x60, 254);

        // Score
        sprintf(buffer, "%05d", _score);
        x -= (int)strlen(buffer) * CHAR_W + CHAR_W;
        mode->drawString(abs_x + x, abs_y, 0xE0, buffer);

        // Hearts / game over (left side)
        if (_health <= 0 || _seconds <= 0)
        {
            mode->drawString(abs_x, abs_y, 0xF0, "GAME OVER!");
        }
        else
        {
            for (int n = 0; n < _health; n++)
                mode->drawChar(abs_x + n * CHAR_W, abs_y, 0x40, 3);
        }
    }
}
