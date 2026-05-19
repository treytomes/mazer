#include "BlockCounter.h"
#include <stdio.h>

namespace UI
{
    BlockCounter::BlockCounter(int x, int y, const int& block_count)
        : Widget(x, y), _block_count(block_count)
    {
    }

    void BlockCounter::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        // Block tile icon followed by count.
        mode->drawChar(abs_x, abs_y, 0x60, 254);
        char buffer[4];
        sprintf(buffer, "x%d", _block_count);
        mode->drawString(abs_x + 8, abs_y, 0xE0, buffer);
    }
}
