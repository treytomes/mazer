#include "ScoreScreen.h"
#include "../KEYBOARD.h"
#include "../AUDIO/SfxPlayer.h"
#include "../AUDIO/SfxParams.h"
#include <stdio.h>
#include <string.h>

namespace UI
{
    static const int HEALTH_SCORE   = 20;
    static const int TIME_SCORE     = 2;
    static const int FINISH_BONUS   = 1000;
    static const int BLINK_SPEED_MS = 500;

    ScoreScreen::ScoreScreen(int screen_width, int screen_height,
                             int health, int time_remaining, int score,
                             bool finished_maze)
        : Widget(0, 0),
          _health(health), _time(time_remaining), _score(score),
          _finish_bonus(finished_maze ? FINISH_BONUS : 0),
          _final_score(0), _consume_speed_ms(300), _consume_timer(0),
          _tally_done(false), _play_again(false),
          _speed_multiplier(1),
          _blink_timer(0), _blink_visible(false),
          _screen_width(screen_width), _screen_height(screen_height)
    {
    }

    void ScoreScreen::setSpeedMultiplier(int multiplier)
    {
        _speed_multiplier = multiplier;
    }

    bool ScoreScreen::playAgain() const
    {
        return _play_again;
    }

    void ScoreScreen::updateSelf(Uint32 dt)
    {
        if (_tally_done)
        {
            _blink_timer += dt;
            if (_blink_timer >= BLINK_SPEED_MS)
            {
                _blink_timer -= BLINK_SPEED_MS;
                _blink_visible = !_blink_visible;
            }

            // Y/Return/A = play again; N/Escape/B = back to menu.
            Uint8 key = KEYBOARD::getScanCode();
            if (key == SDL_SCANCODE_Y || key == SDL_SCANCODE_RETURN || key == SDL_SCANCODE_KP_ENTER)
            {
                _play_again = true;
                close();
            }
            else if (key == SDL_SCANCODE_N || key == SDL_SCANCODE_ESCAPE)
            {
                _play_again = false;
                close();
            }
            return;
        }

        _consume_timer += dt * _speed_multiplier;
        if (_consume_timer < (Uint32)_consume_speed_ms)
            return;
        _consume_timer -= _consume_speed_ms;

        if (_health > 0)
        {
            _health--;
            _final_score += HEALTH_SCORE;
            AUDIO::SfxPlayer::play(AUDIO::Presets::ScoreTick());
        }
        else if (_time > 0)
        {
            _time--;
            _final_score += TIME_SCORE;
            _consume_speed_ms = 80;
            AUDIO::SfxPlayer::play(AUDIO::Presets::ScoreTick());
        }
        else if (_score > 0)
        {
            _score--;
            _final_score++;
            _consume_speed_ms = 8;
            AUDIO::SfxPlayer::play(AUDIO::Presets::ScoreTick());
        }
        else if (_finish_bonus > 0)
        {
            _finish_bonus--;
            _final_score++;
            _consume_speed_ms = 4;
            AUDIO::SfxPlayer::play(AUDIO::Presets::ScoreTick());
        }
        else
        {
            _tally_done = true;
            AUDIO::SfxPlayer::play(AUDIO::Presets::ScoreFinish());
        }
    }

    void ScoreScreen::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        int cx = abs_x + _screen_width  / 2;
        int cy = abs_y + _screen_height / 2;

        char buffer[40];

        sprintf(buffer, "%2dx HEARTS: %d",  HEALTH_SCORE, _health);
        mode->drawString(cx - (int)strlen(buffer) * 4, cy - 4 - 8 * 5, 0xF0, buffer);

        sprintf(buffer, "%2dx TIME: %d", TIME_SCORE, _time);
        mode->drawString(cx - (int)strlen(buffer) * 4, cy - 4 - 8 * 4, 0xF0, buffer);

        sprintf(buffer, " 1x SCORE: %d", _score);
        mode->drawString(cx - (int)strlen(buffer) * 4, cy - 4 - 8 * 3, 0xF0, buffer);

        if (_finish_bonus > 0)
        {
            sprintf(buffer, "FINISHED MAZE: +%d", _finish_bonus);
            mode->drawString(cx - (int)strlen(buffer) * 4, cy - 4 - 8 * 2, 0xF0, buffer);
        }

        sprintf(buffer, "FINAL SCORE: %d", _final_score);
        mode->drawString(cx - (int)strlen(buffer) * 8 / 2, cy - 4, 0xF0, buffer);

        if (_tally_done && _blink_visible)
            mode->drawString(cx - 4 * 17, cy - 4 + 8 * 4, 0xF0, "PLAY AGAIN? (Y/N)");
    }
}
