#pragma once

#include "Widget.h"

namespace UI
{
    class HeartMeter : public Widget
    {
    private:
        const int& _health;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;

    public:
        HeartMeter(int x, int y, const int& health);
    };
}
