#include "BgmParams.h"
#include "SfxParams.h"

namespace AUDIO
{
namespace BgmThemes
{
    BgmParams Menu()
    {
        BgmParams p{};
        p.bpm              = 76.0f;
        p.scale            = SCALE_MAJOR;
        p.root_midi        = 48; // C3
        p.chords[0]        = 0;  // I
        p.chords[1]        = 3;  // IV
        p.chords[2]        = 4;  // V
        p.chords[3]        = 3;  // IV
        p.chord_count      = 4;
        p.melody_density   = 0.6f;
        p.subdivisions     = 2;  // eighth notes
        p.melody_wave      = WAVE_SINE;
        p.melody_volume    = 0.35f;
        p.bass_wave        = WAVE_SINE;
        p.bass_volume      = 0.3f;
        p.melody_octave_span = 2;
        return p;
    }

    BgmParams Game()
    {
        BgmParams p{};
        p.bpm              = 110.0f;
        p.scale            = SCALE_PENTATONIC_MINOR;
        p.root_midi        = 45; // A2
        p.chords[0]        = 0;  // i
        p.chords[1]        = 2;  // III
        p.chords[2]        = 3;  // iv
        p.chords[3]        = 0;  // i
        p.chords[4]        = 4;  // VII
        p.chords[5]        = 0;  // i
        p.chord_count      = 6;
        p.melody_density   = 0.7f;
        p.subdivisions     = 4;  // 16th notes
        p.melody_wave      = WAVE_SQUARE;
        p.melody_volume    = 0.3f;
        p.bass_wave        = WAVE_SQUARE;
        p.bass_volume      = 0.28f;
        p.melody_octave_span = 2;
        return p;
    }

    BgmParams Tense()
    {
        BgmParams p{};
        p.bpm              = 148.0f;
        p.scale            = SCALE_NATURAL_MINOR;
        p.root_midi        = 45; // A2
        p.chords[0]        = 0;  // i
        p.chords[1]        = 6;  // VII
        p.chords[2]        = 5;  // VI
        p.chords[3]        = 4;  // v
        p.chord_count      = 4;
        p.melody_density   = 0.8f;
        p.subdivisions     = 4;  // 16th notes
        p.melody_wave      = WAVE_SAW;
        p.melody_volume    = 0.28f;
        p.bass_wave        = WAVE_SAW;
        p.bass_volume      = 0.25f;
        p.melody_octave_span = 2;
        return p;
    }
}
}
