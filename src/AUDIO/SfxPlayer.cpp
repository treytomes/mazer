#include "SfxPlayer.h"
#include "SfxGenerator.h"
#include "AudioSystem.h"

namespace AUDIO
{
    void SfxPlayer::play(const SfxParams& p, int sample_rate)
    {
        auto samples = SfxGenerator::generate(p, sample_rate);
        AudioSystem::instance().play(std::move(samples));
    }
}
