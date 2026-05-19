#pragma once

#include "Widget.h"

namespace UI
{
    class ScoreScreen : public Widget
    {
    private:
        // Snapshot values counted down during the tally animation.
        int _health;
        int _time;
        int _score;
        int _finish_bonus;

        int _final_score;
        int _consume_speed_ms;
        Uint32 _consume_timer;

        bool _tally_done;
        bool _play_again;

        // Blink state for "PLAY AGAIN?" prompt.
        Uint32 _blink_timer;
        bool _blink_visible;

        int _screen_width;
        int _screen_height;

    protected:
        void drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y) override;
        void updateSelf(Uint32 dt) override;

    public:
        ScoreScreen(int screen_width, int screen_height,
                    int health, int time_remaining, int score,
                    bool finished_maze);

        // Valid after wantsClose() returns true.
        bool playAgain() const;
    };
}
