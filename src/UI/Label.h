#pragma once

#include "Widget.h"

namespace UI
{
    class Label : public Widget
    {
    private:
        const char* _text;
        Uint8 _color;
        int _blink_interval_ms;   // 0 = no blink
        Uint32 _blink_timer;
        bool _blink_visible;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    public:
        Label(int x, int y, const char* text, Uint8 color, int blink_interval_ms = 0);

        void setText(const char* text);
        void setColor(Uint8 color);
    };
}
