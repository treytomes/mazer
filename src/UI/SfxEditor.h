#pragma once

#include "Widget.h"
#include "../AUDIO/SfxParams.h"

namespace UI
{
    class SfxEditor : public Widget
    {
    private:
        int _screen_width;
        int _screen_height;

        int _preset_index;   // Which named preset is being edited
        AUDIO::SfxParams _params;

        int _selected_param; // Which row (0..NUM_PARAMS-1) is highlighted
        int _scroll_offset;  // First visible row index

        static const int VISIBLE_ROWS = 16;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    public:
        SfxEditor(int screen_width, int screen_height);
    };
}
