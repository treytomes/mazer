#include "Widget.h"

namespace UI
{
    Widget::Widget(int x, int y)
        : _x(x), _y(y), _child_count(0), _wants_close(false), _visible(true)
    {
        for (int i = 0; i < MAX_CHILDREN; i++)
            _children[i] = nullptr;
    }

    Widget::~Widget()
    {
        for (int i = 0; i < _child_count; i++)
        {
            delete _children[i];
            _children[i] = nullptr;
        }
        _child_count = 0;
    }

    void Widget::addChild(Widget* child)
    {
        if (_child_count >= MAX_CHILDREN)
            return;
        _children[_child_count++] = child;
    }

    void Widget::removeChild(Widget* child)
    {
        for (int i = 0; i < _child_count; i++)
        {
            if (_children[i] == child)
            {
                delete _children[i];
                // Shift remaining children down.
                for (int j = i; j < _child_count - 1; j++)
                    _children[j] = _children[j + 1];
                _children[--_child_count] = nullptr;
                return;
            }
        }
    }

    void Widget::close()
    {
        _wants_close = true;
    }

    bool Widget::wantsClose() const
    {
        return _wants_close;
    }

    void Widget::setVisible(bool visible)
    {
        _visible = visible;
    }

    bool Widget::isVisible() const
    {
        return _visible;
    }

    void Widget::update(Uint32 dt)
    {
        if (!_visible)
            return;

        updateSelf(dt);

        for (int i = 0; i < _child_count; i++)
            _children[i]->update(dt);

        // Remove in reverse order so index shifts don't cause skips.
        for (int i = _child_count - 1; i >= 0; i--)
        {
            if (_children[i]->wantsClose())
                removeChild(_children[i]);
        }
    }

    void Widget::draw(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        if (!_visible)
            return;

        int wx = abs_x + _x;
        int wy = abs_y + _y;
        drawSelf(mode, wx, wy);
        for (int i = 0; i < _child_count; i++)
            _children[i]->draw(mode, wx, wy);
    }
}
