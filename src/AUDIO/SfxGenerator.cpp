#include "SfxGenerator.h"
#include <cmath>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace AUDIO
{
    // Map normalized 0-1 param to Hz using an exponential curve
    // (matches Bfxr: ~20 Hz at 0, ~8372 Hz at 1).
    static float paramToFreq(float t)
    {
        return 20.0f * powf(8372.0f / 20.0f, t);
    }

    std::vector<float> SfxGenerator::generate(const SfxParams& p, int sample_rate)
    {
        // Total duration in seconds
        float duration = p.attack_time * 1.0f + p.sustain_time * 1.0f + p.decay_time * 1.0f;
        if (duration <= 0.0f) duration = 0.5f;

        int total_samples = (int)(duration * sample_rate);
        std::vector<float> out;
        out.reserve(total_samples);

        float freq      = paramToFreq(p.base_freq);
        float freq_min  = paramToFreq(p.freq_limit);
        float phase     = 0.0f;
        float duty      = p.duty;

        // LPF state
        float lpf_y  = 0.0f;
        float lpf_dp = 0.0f;
        float hpf_y  = 0.0f;

        // Vibrato
        float vib_phase = 0.0f;

        // Arpeggio
        float arp_freq_mult = 1.0f;
        int arp_time   = (p.arp_speed > 0.0f)
                       ? (int)((1.0f - p.arp_speed) * (1.0f - p.arp_speed) * (float)sample_rate * 0.5f)
                       : 0;
        bool arp_fired = false;

        // Repeat
        int repeat_time = (p.repeat_speed > 0.0f)
                        ? (int)((1.0f - p.repeat_speed) * (1.0f - p.repeat_speed) * (float)sample_rate * 0.5f)
                        : 0;
        int repeat_counter = 0;

        // Envelope times in samples
        int env_attack  = (int)(p.attack_time  * sample_rate);
        int env_sustain = (int)(p.sustain_time  * sample_rate);
        int env_decay   = (int)(p.decay_time   * sample_rate);

        // LPF coefficients (mapped from 0–1 to useful range)
        float lpf_cutoff = powf(p.lpf_freq, 3.0f) * 0.1f + 0.001f;
        float hpf_cutoff = powf(p.hpf_freq, 2.0f) * 0.1f;

        float noise_last = 0.0f;
        int noise_count  = 0;

        for (int i = 0; i < total_samples; i++)
        {
            // Repeat: reset oscillator state periodically
            if (repeat_time > 0)
            {
                repeat_counter++;
                if (repeat_counter >= repeat_time)
                {
                    repeat_counter = 0;
                    freq      = paramToFreq(p.base_freq);
                    duty      = p.duty;
                    arp_fired = false;
                    phase     = 0.0f;
                }
            }

            // Arpeggio
            if (arp_time > 0 && !arp_fired && i >= arp_time)
            {
                arp_fired    = true;
                arp_freq_mult = (p.arp_mod >= 0.0f)
                              ? 1.0f + p.arp_mod * p.arp_mod * 10.0f
                              : 1.0f / (1.0f + (-p.arp_mod) * (-p.arp_mod) * 10.0f);
            }
            float cur_freq = freq * arp_freq_mult;

            // Frequency ramp
            freq += freq * p.freq_ramp * 0.0001f;
            freq += freq * p.freq_dramp * 0.0001f * p.freq_ramp;
            if (p.freq_limit > 0.0f && cur_freq < freq_min)
                cur_freq = freq_min;

            // Vibrato
            vib_phase += p.vib_speed * 0.01f;
            cur_freq  *= 1.0f + sinf(vib_phase) * p.vib_strength * 0.5f;

            // Duty ramp
            duty += p.duty_ramp * 0.00005f;
            if (duty < 0.0f) duty = 0.0f;
            if (duty > 1.0f) duty = 1.0f;

            // Advance oscillator
            float period = (float)sample_rate / cur_freq;
            phase += 1.0f;
            if (phase >= period) phase -= period;
            float t = phase / period; // 0–1

            // Waveform
            float sample = 0.0f;
            switch (p.wave_type)
            {
                case WAVE_SQUARE:
                    sample = (t < duty) ? 1.0f : -1.0f;
                    break;
                case WAVE_SAW:
                    sample = 1.0f - t * 2.0f;
                    break;
                case WAVE_SINE:
                    sample = sinf(t * 2.0f * (float)M_PI);
                    break;
                case WAVE_TRIANGLE:
                    sample = (t < 0.5f) ? (t * 4.0f - 1.0f) : (3.0f - t * 4.0f);
                    break;
                case WAVE_NOISE:
                default:
                    noise_count++;
                    if (noise_count >= (int)(period / 4.0f + 1))
                    {
                        noise_count = 0;
                        noise_last = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
                    }
                    sample = noise_last;
                    break;
            }

            // Low-pass filter
            lpf_cutoff *= (1.0f + p.lpf_ramp * 0.0001f);
            if (lpf_cutoff < 0.001f) lpf_cutoff = 0.001f;
            if (lpf_cutoff > 0.1f)  lpf_cutoff = 0.1f;
            float lpf_resonance = p.lpf_resonance * 10.0f;
            lpf_dp  += (sample - lpf_y) * lpf_cutoff;
            lpf_dp  -= lpf_dp * lpf_resonance * 0.01f;
            lpf_y   += lpf_dp;
            sample   = lpf_y;

            // High-pass filter
            hpf_cutoff *= (1.0f + p.hpf_ramp * 0.0001f);
            if (hpf_cutoff < 0.0f)    hpf_cutoff = 0.0f;
            if (hpf_cutoff > 0.1f)    hpf_cutoff = 0.1f;
            hpf_y  += hpf_cutoff * (hpf_y - sample) * -1.0f;
            float hp_out = sample - hpf_y;
            hpf_y       = sample - hp_out;
            sample       = hp_out;

            // Envelope
            float env = 0.0f;
            if (i < env_attack)
            {
                env = (float)i / (float)(env_attack > 0 ? env_attack : 1);
            }
            else if (i < env_attack + env_sustain)
            {
                int si = i - env_attack;
                env = 1.0f + p.sustain_punch * (1.0f - (float)si / (float)(env_sustain > 0 ? env_sustain : 1));
            }
            else
            {
                int di = i - env_attack - env_sustain;
                float t_decay = (float)di / (float)(env_decay > 0 ? env_decay : 1);
                env = 1.0f - t_decay;
                if (env < 0.0f) env = 0.0f;
            }

            sample *= env * p.volume;

            // Hard clip
            if (sample >  1.0f) sample =  1.0f;
            if (sample < -1.0f) sample = -1.0f;

            out.push_back(sample);
        }

        return out;
    }
}
