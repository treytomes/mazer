#pragma once

#include "SfxParams.h"
#include <vector>

namespace AUDIO
{
    class SfxGenerator
    {
    public:
        // Synthesize PCM samples from params. Returns normalized [-1, 1] floats
        // at the given sample_rate. The returned vector is the full sound clip.
        static std::vector<float> generate(const SfxParams& p, int sample_rate = 44100);
    };
}
