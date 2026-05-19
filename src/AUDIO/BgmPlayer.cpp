#include "BgmPlayer.h"
#include "SfxParams.h"
#include "SfxGenerator.h"
#include "AudioSystem.h"
#include <cmath>

namespace AUDIO
{
    // Semitone intervals for each scale type, per degree (0-based, 7 degrees).
    static const int SCALE_INTERVALS[3][7] = {
        { 0, 2, 4, 5, 7, 9, 11 }, // major
        { 0, 3, 5, 7, 10, 0, 0 }, // pentatonic minor (5 notes; degrees 5&6 unused)
        { 0, 2, 3, 5, 7, 8, 10 }, // natural minor
    };
    static const int SCALE_DEGREES[3] = { 7, 5, 7 };

    // Convert MIDI note number to normalized base_freq for SfxParams.
    // MIDI 69 = A4 = 440 Hz.  Bfxr freq range: ~20–8372 Hz.
    static float midiToBaseFreq(int midi)
    {
        float hz = 440.0f * powf(2.0f, (midi - 69) / 12.0f);
        float t  = logf(hz / 20.0f) / logf(8372.0f / 20.0f);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return t;
    }

    // Build an SfxParams for a short note (note_dur_ms long) on one voice.
    static SfxParams makeNote(int midi, float note_dur_s,
                              int wave_type, float volume)
    {
        SfxParams p{};
        p.wave_type     = wave_type;
        p.attack_time   = 0.01f;
        p.sustain_time  = note_dur_s * 0.5f;
        p.sustain_punch = 0.0f;
        p.decay_time    = note_dur_s * 0.5f;
        p.base_freq     = midiToBaseFreq(midi);
        p.freq_limit    = 0.0f;
        p.freq_ramp     = 0.0f;
        p.freq_dramp    = 0.0f;
        p.vib_strength  = 0.0f;
        p.vib_speed     = 0.0f;
        p.arp_mod       = 0.0f;
        p.arp_speed     = 0.0f;
        p.duty          = 0.5f;
        p.duty_ramp     = 0.0f;
        p.repeat_speed  = 0.0f;
        p.lpf_freq      = 1.0f;
        p.lpf_ramp      = 0.0f;
        p.lpf_resonance = 0.0f;
        p.hpf_freq      = 0.0f;
        p.hpf_ramp      = 0.0f;
        p.volume        = volume;
        return p;
    }

    BgmPlayer::BgmPlayer()
        : _playing(false),
          _tick_ms(0.0f), _tick_accum(0.0f),
          _beat(0), _beats_per_measure(16),
          _chord_index(0), _chord_beat(0),
          _melody_last_degree(0),
          _rng(0xDEADBEEF)
    {
    }

    float BgmPlayer::nextRand()
    {
        _rng = _rng * 1664525u + 1013904223u;
        return (_rng >> 8) / (float)(1 << 24);
    }

    int BgmPlayer::nextInt(int n)
    {
        if (n <= 1) return 0;
        return (int)(nextRand() * n) % n;
    }

    void BgmPlayer::setParams(const BgmParams& p)
    {
        _params = p;

        // ms per subdivision tick
        float beat_ms  = 60000.0f / p.bpm;
        _tick_ms       = beat_ms / p.subdivisions;

        _beats_per_measure = p.subdivisions * 4; // 4/4 time
        _playing       = true;
        _beat          = 0;
        _tick_accum    = 0.0f;
        _chord_index   = 0;
        _chord_beat    = 0;
        _melody_last_degree = 0;
    }

    void BgmPlayer::stop()
    {
        _playing = false;
    }

    int BgmPlayer::scaleDegreeToMidi(int root_midi, int degree) const
    {
        int scale_idx = (int)_params.scale;
        int n_degrees = SCALE_DEGREES[scale_idx];
        int octave    = degree / n_degrees;
        int d         = degree % n_degrees;
        return root_midi + octave * 12 + SCALE_INTERVALS[scale_idx][d];
    }

    int BgmPlayer::currentChordRoot() const
    {
        if (_params.chord_count <= 0) return _params.root_midi;
        int degree = _params.chords[_chord_index % _params.chord_count];
        return scaleDegreeToMidi(_params.root_midi, degree);
    }

    void BgmPlayer::fireMelodyNote(int chord_root_midi)
    {
        // Note duration = one subdivision's worth of time.
        float note_dur_s = _tick_ms / 1000.0f * 0.85f; // 85% legato

        // Scale-degree pool: prefer chord tones (root, 3rd, 5th in scale steps).
        // Do a random walk within +/-2 degrees of last position.
        int scale_idx = (int)_params.scale;
        int n_degrees = SCALE_DEGREES[scale_idx];
        int span      = _params.melody_octave_span * n_degrees;

        int delta = nextInt(5) - 2; // -2 to +2
        int next  = _melody_last_degree + delta;
        if (next < 0)   next = 0;
        if (next > span) next = span;
        _melody_last_degree = next;

        // Bias toward chord root: 25% chance to snap to a chord tone.
        if (nextRand() < 0.25f)
        {
            int chord_degree = _params.chords[_chord_index % _params.chord_count];
            _melody_last_degree = chord_degree + nextInt(_params.melody_octave_span) * n_degrees;
        }

        // Shift the note up one octave from the chord root so melody sits above bass.
        int midi = scaleDegreeToMidi(chord_root_midi - _params.root_midi
                                     + _params.root_midi + 12,
                                     _melody_last_degree % (n_degrees * 2));

        SfxParams sp = makeNote(midi, note_dur_s,
                                _params.melody_wave, _params.melody_volume);
        auto samples = SfxGenerator::generate(sp, 44100);
        AudioSystem::instance().play(std::move(samples));
    }

    void BgmPlayer::fireBassNote(int chord_root_midi)
    {
        // Bass plays on beat 1 and beat 3 of each measure (quarter-note feel).
        float note_dur_s = (_tick_ms * _params.subdivisions) / 1000.0f * 0.7f;

        // Bass is the chord root, one octave below.
        int midi = chord_root_midi - 12;

        SfxParams sp = makeNote(midi, note_dur_s,
                                _params.bass_wave, _params.bass_volume);
        auto samples = SfxGenerator::generate(sp, 44100);
        AudioSystem::instance().play(std::move(samples));
    }

    void BgmPlayer::update(Uint32 dt)
    {
        if (!_playing) return;

        _tick_accum += (float)dt;

        while (_tick_accum >= _tick_ms)
        {
            _tick_accum -= _tick_ms;

            int chord_root = currentChordRoot();

            // Bass: fire on beat 0 and beat (subdivisions*2) within the measure.
            int half = _params.subdivisions * 2;
            if (_beat == 0 || _beat == half)
                fireBassNote(chord_root);

            // Melody: fire probabilistically each subdivision.
            if (nextRand() < _params.melody_density)
                fireMelodyNote(chord_root);

            _beat++;
            if (_beat >= _beats_per_measure)
            {
                _beat = 0;

                // Advance chord every 1 or 2 measures (alternating adds variety).
                _chord_beat++;
                int measures_per_chord = (_chord_index % 2 == 0) ? 2 : 1;
                if (_chord_beat >= measures_per_chord)
                {
                    _chord_beat  = 0;
                    _chord_index = (_chord_index + 1) % _params.chord_count;
                }
            }
        }
    }
}
