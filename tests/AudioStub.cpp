// Null SfxPlayer for tests — avoids linking AudioSystem (which opens an SDL audio device).
#include "AUDIO/SfxPlayer.h"

namespace AUDIO
{
    void SfxPlayer::play(const SfxParams& /*p*/, int /*sample_rate*/) {}
}
