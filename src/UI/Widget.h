#pragma once

#include <SDL.h>
#include "../CGA/GraphicsMode.h"

namespace UI
{
    const int MAX_CHILDREN = 16;

    class Widget
    {
    private:
        Widget* _children[MAX_CHILDREN];
        int _child_count;
        bool _wants_close;
        bool _visible;

    protected:
        int _x;
        int _y;

        virtual void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) {}
        virtual void updateSelf(Uint32 dt) {}

    public:
        Widget(int x, int y);
        virtual ~Widget();

        void addChild(Widget* child);
        void removeChild(Widget* child);

        void close();
        bool wantsClose() const;

        void setVisible(bool visible);
        bool isVisible() const;

        void update(Uint32 dt);
        void draw(CGA::GraphicsMode* mode, int abs_x = 0, int abs_y = 0);
    };
}
