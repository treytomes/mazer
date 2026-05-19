#pragma once

#include <SDL2/SDL.h>
#include <vector>

namespace AUDIO
{
    // Maximum simultaneously-playing voices.
    static const int MAX_VOICES = 16;

    struct Voice
    {
        std::vector<float> samples;
        int position;
        bool active;
    };

    class AudioSystem
    {
    public:
        static AudioSystem& instance();

        bool initialize();
        void shutdown();

        // Push a synthesized clip to the mixer. Thread-safe.
        void play(std::vector<float>&& samples);

    private:
        AudioSystem();
        ~AudioSystem();
        AudioSystem(const AudioSystem&) = delete;
        AudioSystem& operator=(const AudioSystem&) = delete;

        static void audioCallback(void* userdata, Uint8* stream, int len);
        void fillBuffer(float* buf, int frame_count);

        SDL_AudioDeviceID _device;
        SDL_mutex*        _mutex;
        Voice             _voices[MAX_VOICES];
        int               _next_voice;
        int               _sample_rate;
    };
}
