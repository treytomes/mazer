#pragma once

namespace AUDIO
{
    enum WaveType { WAVE_SQUARE = 0, WAVE_SAW, WAVE_SINE, WAVE_TRIANGLE, WAVE_NOISE };

    struct SfxParams
    {
        int   wave_type;        // WaveType enum

        float attack_time;      // 0.0–1.0
        float sustain_time;     // 0.0–1.0
        float sustain_punch;    // 0.0–1.0
        float decay_time;       // 0.0–1.0

        float base_freq;        // 0.0–1.0
        float freq_limit;       // 0.0–1.0
        float freq_ramp;        // -1.0–1.0
        float freq_dramp;       // -1.0–1.0

        float vib_strength;     // 0.0–1.0
        float vib_speed;        // 0.0–1.0

        float arp_mod;          // -1.0–1.0
        float arp_speed;        // 0.0–1.0

        float duty;             // 0.0–1.0 (square wave duty cycle)
        float duty_ramp;        // -1.0–1.0

        float repeat_speed;     // 0.0–1.0

        float lpf_freq;         // 0.0–1.0
        float lpf_ramp;         // -1.0–1.0
        float lpf_resonance;    // 0.0–1.0

        float hpf_freq;         // 0.0–1.0
        float hpf_ramp;         // -1.0–1.0

        float volume;           // 0.0–1.0
    };

    // Named preset accessors — all return a fully-initialized SfxParams.
    namespace Presets
    {
        SfxParams WhirligigHit();
        SfxParams PickupFood();
        SfxParams PickupClock();
        SfxParams PickupHealth();
        SfxParams MazeFinish();
        SfxParams MenuNavigate();
        SfxParams MenuSelect();
        SfxParams PowerDown();
        SfxParams ScoreTick();
        SfxParams ScoreFinish();

        // Total number of named presets (used for cycling in SfxEditor).
        static const int COUNT = 10;

        // Returns preset by index [0, COUNT).
        SfxParams get(int index);

        // Returns the display name by index.
        const char* name(int index);
    }
}
