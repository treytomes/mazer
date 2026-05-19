#include "Label.h"

namespace UI
{
    Label::Label(int x, int y, const char* text, Uint8 color, int blink_interval_ms)
        : Widget(x, y), _text(text), _color(color),
          _blink_interval_ms(blink_interval_ms), _blink_timer(0), _blink_visible(true)
    {
    }

    void Label::setText(const char* text)
    {
        _text = text;
    }

    void Label::setColor(Uint8 color)
    {
        _color = color;
    }

    void Label::updateSelf(Uint32 dt)
    {
        if (_blink_interval_ms <= 0)
            return;

        _blink_timer += dt;
        if (_blink_timer >= (Uint32)_blink_interval_ms)
        {
            _blink_timer -= _blink_interval_ms;
            _blink_visible = !_blink_visible;
        }
    }

    void Label::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        if (_blink_interval_ms > 0 && !_blink_visible)
            return;
        if (_text == nullptr)
            return;

        mode->drawString(abs_x, abs_y, _color, _text);
    }
}
