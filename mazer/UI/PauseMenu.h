#pragma once

#include "Widget.h"

namespace UI
{
    class PauseMenu : public Widget
    {
    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;

    public:
        // screen_width / screen_height used to center the box.
        PauseMenu(int screen_width, int screen_height);
    };
}
