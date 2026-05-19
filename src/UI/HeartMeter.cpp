#include "HeartMeter.h"

namespace UI
{
    HeartMeter::HeartMeter(int x, int y, const int& health)
        : Widget(x, y), _health(health)
    {
    }

    void HeartMeter::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        for (int n = 0; n < _health; n++)
            mode->drawChar(abs_x + n * 8, abs_y, 0x40, 3);
    }
}
