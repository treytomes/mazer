#pragma once

#include "Widget.h"

namespace UI
{
    enum class CurtainState { Idle, Opening, Closing };

    class CurtainTransition : public Widget
    {
    private:
        int _screen_width;
        int _screen_height;
        int _curtain_x;       // width of each side panel; 0 = fully open
        CurtainState _state;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    public:
        CurtainTransition(int screen_width, int screen_height);

        void open();
        void closeScreen();
        bool isDone() const;
    };
}
