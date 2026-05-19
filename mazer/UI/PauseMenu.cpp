#include "PauseMenu.h"
#include "BorderedBox.h"
#include "Label.h"

namespace UI
{
    PauseMenu::PauseMenu(int screen_width, int screen_height)
        : Widget(0, 0)
    {
        int left   = (int)(screen_width  * 0.3f);
        int top    = (int)(screen_height * 0.3f);
        int width  = (int)(screen_width  * 0.4f);
        int height = (int)(screen_height * 0.4f);
        int center_x = screen_width  / 2;
        int center_y = screen_height / 2;

        addChild(new BorderedBox(left, top, width, height, 9, 0));

        // "PAUSED!" centered in the box.
        addChild(new Label(center_x - 4 * 7, center_y - 4, "PAUSED!", 0xF0));

        // Blinking "PRESS <TAB>" hint, 500ms interval.
        addChild(new Label(center_x - 4 * 11, center_y - 4 + 8 * 4, "PRESS <TAB>", 0xF0, 500));
    }

    void PauseMenu::drawSelf(CGA::GraphicsMode* /*mode*/, int /*abs_x*/, int /*abs_y*/)
    {
        // All rendering handled by children.
    }
}
