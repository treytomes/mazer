#pragma once

#include "BgmParams.h"
#include <SDL2/SDL.h>

namespace AUDIO
{
    class BgmPlayer
    {
    public:
        BgmPlayer();

        void setParams(const BgmParams& p);
        void stop();

        // Call every frame with the frame delta in milliseconds.
        void update(Uint32 dt);

        bool isPlaying() const { return _playing; }

    private:
        void fireMelodyNote(int chord_root_midi);
        void fireBassNote(int chord_root_midi);

        // Returns a MIDI pitch for one scale degree above root, given the
        // current params.  degree is 0-based.
        int scaleDegreeToMidi(int root_midi, int degree) const;

        // Returns the MIDI note of the current chord's root.
        int currentChordRoot() const;

        BgmParams _params;
        bool      _playing;

        // Beat clock (in ms per subdivision tick).
        float     _tick_ms;        // ms per subdivision
        float     _tick_accum;     // accumulated ms since last tick

        int       _beat;           // current subdivision index (resets each measure)
        int       _beats_per_measure; // subdivisions * 4 (one bar of 4/4)

        int       _chord_index;    // current position in chord progression
        int       _chord_beat;     // how many measures we've been on this chord

        // Melody voice: small random walk state.
        int       _melody_last_degree; // last scale degree played (for smooth walk)

        // Simple LCG for deterministic randomness that doesn't affect game rand().
        Uint32    _rng;
        float     nextRand();  // returns 0.0–1.0
        int       nextInt(int n); // returns 0..n-1
    };
}
