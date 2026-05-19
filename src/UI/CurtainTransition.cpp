#include "CurtainTransition.h"

namespace UI
{
    // Pixels the curtain moves per update tick.
    static const int CURTAIN_SPEED = 2;

    CurtainTransition::CurtainTransition(int screen_width, int screen_height)
        : Widget(0, 0),
          _screen_width(screen_width), _screen_height(screen_height),
          _curtain_x(0), _state(CurtainState::Idle)
    {
    }

    void CurtainTransition::open()
    {
        _curtain_x = _screen_width / 2;
        _state = CurtainState::Opening;
    }

    void CurtainTransition::closeScreen()
    {
        _curtain_x = 0;
        _state = CurtainState::Closing;
    }

    bool CurtainTransition::isDone() const
    {
        return _state == CurtainState::Idle;
    }

    void CurtainTransition::updateSelf(Uint32 /*dt*/)
    {
        if (_state == CurtainState::Opening)
        {
            _curtain_x -= CURTAIN_SPEED;
            if (_curtain_x <= 0)
            {
                _curtain_x = 0;
                _state = CurtainState::Idle;
            }
        }
        else if (_state == CurtainState::Closing)
        {
            _curtain_x += CURTAIN_SPEED;
            if (_curtain_x >= _screen_width / 2)
            {
                _curtain_x = _screen_width / 2;
                _state = CurtainState::Idle;
            }
        }
    }

    void CurtainTransition::drawSelf(CGA::GraphicsMode* mode, int abs_x, int abs_y)
    {
        if (_curtain_x <= 0)
            return;

        // Left panel
        mode->drawFilledRect(abs_x,
                             abs_x + _curtain_x,
                             abs_y + 8,                  // below HUD
                             abs_y + _screen_height - 1,
                             0);
        // Right panel
        mode->drawFilledRect(abs_x + _screen_width - _curtain_x,
                             abs_x + _screen_width,
                             abs_y + 8,
                             abs_y + _screen_height - 1,
                             0);
    }
}
