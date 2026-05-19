#include "BorderedBox.h"

namespace UI
{
    BorderedBox::BorderedBox(int x, int y, int width, int height, Uint8 border_color, Uint8 fill_color)
        : Widget(x, y), _width(width), _height(height),
          _border_color(border_color), _fill_color(fill_color)
    {
    }

    void BorderedBox::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        int x2 = abs_x + _width - 1;
        int y2 = abs_y + _height - 1;

        mode->drawFilledRect(abs_x, x2, abs_y, y2, _fill_color);

        mode->drawHLine(abs_x, x2, abs_y,     _border_color);
        mode->drawHLine(abs_x, x2, abs_y + 2, _border_color);
        mode->drawHLine(abs_x, x2, y2,        _border_color);
        mode->drawHLine(abs_x, x2, y2 - 2,    _border_color);

        mode->drawVLine(abs_x,     abs_y, y2, _border_color);
        mode->drawVLine(abs_x + 2, abs_y, y2, _border_color);
        mode->drawVLine(x2,        abs_y, y2, _border_color);
        mode->drawVLine(x2 - 2,    abs_y, y2, _border_color);
    }
}
