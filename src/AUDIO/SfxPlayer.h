#pragma once

#include "SfxParams.h"

namespace AUDIO
{
    // Thin facade: generate samples from params and hand them to AudioSystem.
    class SfxPlayer
    {
    public:
        static void play(const SfxParams& p, int sample_rate = 44100);
    };
}
