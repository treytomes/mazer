#pragma once

namespace AUDIO
{
    enum ScaleType
    {
        SCALE_MAJOR = 0,
        SCALE_PENTATONIC_MINOR,
        SCALE_NATURAL_MINOR,
    };

    // Chord progression as an array of scale-degree indices (0-based).
    // Max 8 chords; unused slots should be -1.
    static const int BGM_MAX_CHORDS = 8;

    struct BgmParams
    {
        float bpm;
        ScaleType scale;
        int root_midi;           // MIDI note of the tonic (e.g. 48 = C3)

        // Chord progression: each entry is a scale-degree index (0–6).
        int chords[BGM_MAX_CHORDS];
        int chord_count;

        // Per-beat probability that the melody plays a note (0.0–1.0).
        float melody_density;
        // Melody plays on 16th-note subdivisions (1 = quarter, 2 = eighth, 4 = 16th).
        int subdivisions;

        // Wave types and volumes for the two voices.
        int   melody_wave;      // WaveType
        float melody_volume;
        int   bass_wave;        // WaveType
        float bass_volume;

        // Octave range melody notes are chosen within (above root_midi).
        int melody_octave_span; // e.g. 2 → two octaves above root
    };

    namespace BgmThemes
    {
        BgmParams Menu();
        BgmParams Game();
        BgmParams Tense();
    }
}
