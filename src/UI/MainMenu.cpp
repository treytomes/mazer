#include "MainMenu.h"
#include "../KEYBOARD.h"
#include <stdio.h>
#include <string.h>

namespace UI
{
    static const int   NUM_ITEMS      = 2;
    static const char* ITEM_LABELS[NUM_ITEMS] = { "Play", "Quit" };
    static const int   BLINK_SPEED_MS = 400;
    static const int   CHAR_W         = 8;
    static const int   CHAR_H         = 8;

    MainMenu::MainMenu(int screen_width, int screen_height)
        : Widget(0, 0),
          _screen_width(screen_width), _screen_height(screen_height),
          _selected(0), _result(MainMenuResult::None),
          _blink_timer(0), _blink_visible(true)
    {
    }

    MainMenuResult MainMenu::result() const
    {
        return _result;
    }

    void MainMenu::updateSelf(Uint32 dt)
    {
        _blink_timer += dt;
        if (_blink_timer >= (Uint32)BLINK_SPEED_MS)
        {
            _blink_timer -= BLINK_SPEED_MS;
            _blink_visible = !_blink_visible;
        }

        Uint8 key = KEYBOARD::getScanCode();
        if (key == SDL_SCANCODE_UP)
        {
            _selected = (_selected - 1 + NUM_ITEMS) % NUM_ITEMS;
            KEYBOARD::setScanCode(0);
        }
        else if (key == SDL_SCANCODE_DOWN)
        {
            _selected = (_selected + 1) % NUM_ITEMS;
            KEYBOARD::setScanCode(0);
        }
        else if (key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_KP_ENTER)
        {
            _result = (_selected == 0) ? MainMenuResult::Play : MainMenuResult::Quit;
            close();
        }
    }

    void MainMenu::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        const int cx = abs_x + _screen_width  / 2;
        const int cy = abs_y + _screen_height / 2;

        // Title
        const char* title = "MAZER";
        int title_x = cx - (int)strlen(title) * CHAR_W / 2;
        mode->drawString(title_x, cy - CHAR_H * 5, 0xA0, title);

        // Separator line
        const int sep_half = 40;
        mode->drawHLine(cx - sep_half, cx + sep_half, cy - CHAR_H * 3, 0x08);

        // Menu items
        for (int i = 0; i < NUM_ITEMS; i++)
        {
            const char* label = ITEM_LABELS[i];
            int item_y = cy - CHAR_H + i * (CHAR_H + 4);
            bool is_selected = (i == _selected);

            // Colour: bright white when selected, dark grey when not.
            Uint8 color = is_selected ? 0xF0 : 0x80;

            // Blinking cursor marker to the left of the selected item.
            if (is_selected && _blink_visible)
            {
                int marker_x = cx - (int)strlen(label) * CHAR_W / 2 - CHAR_W * 2;
                mode->drawChar(marker_x, item_y, 0xF0, 16, 16); // chr(16) = ►
            }

            int item_x = cx - (int)strlen(label) * CHAR_W / 2;
            mode->drawString(item_x, item_y, color, label);
        }
    }
}
