#include "SfxParams.h"

namespace AUDIO
{
namespace Presets
{
    SfxParams WhirligigHit()
    {
        SfxParams p{};
        p.wave_type      = WAVE_NOISE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.15f;
        p.sustain_punch  = 0.5f;
        p.decay_time     = 0.3f;
        p.base_freq      = 0.4f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = -0.3f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.0f;
        p.arp_speed      = 0.0f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 0.8f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.3f;
        p.hpf_freq       = 0.1f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.6f;
        return p;
    }

    SfxParams PickupFood()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SQUARE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.05f;
        p.sustain_punch  = 0.3f;
        p.decay_time     = 0.2f;
        p.base_freq      = 0.6f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.2f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.0f;
        p.arp_speed      = 0.0f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.4f;
        return p;
    }

    SfxParams PickupClock()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SINE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.1f;
        p.sustain_punch  = 0.4f;
        p.decay_time     = 0.35f;
        p.base_freq      = 0.5f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.15f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.5f;
        p.arp_speed      = 0.5f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.5f;
        return p;
    }

    SfxParams PickupHealth()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SINE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.15f;
        p.sustain_punch  = 0.6f;
        p.decay_time     = 0.4f;
        p.base_freq      = 0.45f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.25f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.1f;
        p.vib_speed      = 0.5f;
        p.arp_mod        = 0.35f;
        p.arp_speed      = 0.55f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.55f;
        return p;
    }

    SfxParams MazeFinish()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SQUARE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.3f;
        p.sustain_punch  = 0.5f;
        p.decay_time     = 0.5f;
        p.base_freq      = 0.35f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.4f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.6f;
        p.arp_speed      = 0.6f;
        p.duty           = 0.3f;
        p.duty_ramp      = 0.1f;
        p.repeat_speed   = 0.4f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.7f;
        return p;
    }

    SfxParams MenuNavigate()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SQUARE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.05f;
        p.sustain_punch  = 0.0f;
        p.decay_time     = 0.1f;
        p.base_freq      = 0.55f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.0f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.0f;
        p.arp_speed      = 0.0f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.35f;
        return p;
    }

    SfxParams MenuSelect()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SQUARE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.08f;
        p.sustain_punch  = 0.3f;
        p.decay_time     = 0.2f;
        p.base_freq      = 0.5f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.3f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.4f;
        p.arp_speed      = 0.5f;
        p.duty           = 0.4f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.5f;
        return p;
    }

    SfxParams PowerDown()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SAW;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.2f;
        p.sustain_punch  = 0.0f;
        p.decay_time     = 0.7f;
        p.base_freq      = 0.55f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = -0.45f;
        p.freq_dramp     = -0.1f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.0f;
        p.arp_speed      = 0.0f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 0.7f;
        p.lpf_ramp       = -0.1f;
        p.lpf_resonance  = 0.2f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.65f;
        return p;
    }

    SfxParams ScoreTick()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SQUARE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.02f;
        p.sustain_punch  = 0.0f;
        p.decay_time     = 0.05f;
        p.base_freq      = 0.65f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.0f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.0f;
        p.arp_speed      = 0.0f;
        p.duty           = 0.5f;
        p.duty_ramp      = 0.0f;
        p.repeat_speed   = 0.0f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.1f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.3f;
        return p;
    }

    SfxParams ScoreFinish()
    {
        SfxParams p{};
        p.wave_type      = WAVE_SQUARE;
        p.attack_time    = 0.0f;
        p.sustain_time   = 0.15f;
        p.sustain_punch  = 0.6f;
        p.decay_time     = 0.5f;
        p.base_freq      = 0.4f;
        p.freq_limit     = 0.0f;
        p.freq_ramp      = 0.3f;
        p.freq_dramp     = 0.0f;
        p.vib_strength   = 0.0f;
        p.vib_speed      = 0.0f;
        p.arp_mod        = 0.55f;
        p.arp_speed      = 0.65f;
        p.duty           = 0.3f;
        p.duty_ramp      = 0.05f;
        p.repeat_speed   = 0.5f;
        p.lpf_freq       = 1.0f;
        p.lpf_ramp       = 0.0f;
        p.lpf_resonance  = 0.0f;
        p.hpf_freq       = 0.0f;
        p.hpf_ramp       = 0.0f;
        p.volume         = 0.65f;
        return p;
    }

    SfxParams get(int index)
    {
        switch (index)
        {
            case 0: return WhirligigHit();
            case 1: return PickupFood();
            case 2: return PickupClock();
            case 3: return PickupHealth();
            case 4: return MazeFinish();
            case 5: return MenuNavigate();
            case 6: return MenuSelect();
            case 7: return PowerDown();
            case 8: return ScoreTick();
            case 9: return ScoreFinish();
            default: return SfxParams{};
        }
    }

    const char* name(int index)
    {
        static const char* names[] = {
            "WhirligigHit",
            "PickupFood",
            "PickupClock",
            "PickupHealth",
            "MazeFinish",
            "MenuNavigate",
            "MenuSelect",
            "PowerDown",
            "ScoreTick",
            "ScoreFinish"
        };
        if (index >= 0 && index < COUNT)
            return names[index];
        return "Unknown";
    }
}
}
