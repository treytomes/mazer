#pragma once

#include "Widget.h"

namespace UI
{
    class BorderedBox : public Widget
    {
    private:
        int _width;
        int _height;
        Uint8 _border_color;
        Uint8 _fill_color;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;

    public:
        BorderedBox(int x, int y, int width, int height, Uint8 border_color, Uint8 fill_color);
    };
}
