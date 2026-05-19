#pragma once

#include "Widget.h"

namespace UI
{
    class HUD : public Widget
    {
    private:
        const int& _health;
        const int& _score;
        const int& _block_count;
        const unsigned char& _seconds;
        int _screen_width;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;

    public:
        HUD(int screen_width,
            const int& health,
            const int& score,
            const int& block_count,
            const unsigned char& seconds);
    };
}
