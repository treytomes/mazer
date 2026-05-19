#pragma once

#include "Widget.h"

namespace UI
{
    class BlockCounter : public Widget
    {
    private:
        const int& _block_count;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;

    public:
        BlockCounter(int x, int y, const int& block_count);
    };
}
