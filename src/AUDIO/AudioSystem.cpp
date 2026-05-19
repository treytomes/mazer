#include "AudioSystem.h"
#include <cstring>

namespace AUDIO
{
    AudioSystem& AudioSystem::instance()
    {
        static AudioSystem inst;
        return inst;
    }

    AudioSystem::AudioSystem()
        : _device(0), _mutex(nullptr), _next_voice(0), _sample_rate(44100)
    {
        for (int i = 0; i < MAX_VOICES; i++)
            _voices[i].active = false;
    }

    AudioSystem::~AudioSystem()
    {
        shutdown();
    }

    bool AudioSystem::initialize()
    {
        _mutex = SDL_CreateMutex();
        if (!_mutex) return false;

        SDL_AudioSpec want{}, got{};
        want.freq     = _sample_rate;
        want.format   = AUDIO_F32SYS;
        want.channels = 1;
        want.samples  = 512;
        want.callback = audioCallback;
        want.userdata = this;

        _device = SDL_OpenAudioDevice(nullptr, 0, &want, &got, 0);
        if (_device == 0)
            return false;

        _sample_rate = got.freq;
        SDL_PauseAudioDevice(_device, 0);
        return true;
    }

    void AudioSystem::shutdown()
    {
        if (_device != 0)
        {
            SDL_CloseAudioDevice(_device);
            _device = 0;
        }
        if (_mutex)
        {
            SDL_DestroyMutex(_mutex);
            _mutex = nullptr;
        }
    }

    void AudioSystem::play(std::vector<float>&& samples)
    {
        if (!_mutex || _device == 0) return;

        SDL_LockMutex(_mutex);

        // Find an inactive slot, or steal the oldest (round-robin).
        int slot = _next_voice;
        for (int i = 0; i < MAX_VOICES; i++)
        {
            int idx = (_next_voice + i) % MAX_VOICES;
            if (!_voices[idx].active)
            {
                slot = idx;
                break;
            }
        }
        _next_voice = (slot + 1) % MAX_VOICES;

        _voices[slot].samples  = std::move(samples);
        _voices[slot].position = 0;
        _voices[slot].active   = true;

        SDL_UnlockMutex(_mutex);
    }

    void AudioSystem::audioCallback(void* userdata, Uint8* stream, int len)
    {
        AudioSystem* self = static_cast<AudioSystem*>(userdata);
        int frame_count   = len / sizeof(float);
        float* buf        = reinterpret_cast<float*>(stream);
        memset(buf, 0, len);
        self->fillBuffer(buf, frame_count);
    }

    void AudioSystem::fillBuffer(float* buf, int frame_count)
    {
        SDL_LockMutex(_mutex);

        for (int i = 0; i < MAX_VOICES; i++)
        {
            Voice& v = _voices[i];
            if (!v.active) continue;

            int remaining = (int)v.samples.size() - v.position;
            int to_mix    = (remaining < frame_count) ? remaining : frame_count;

            for (int s = 0; s < to_mix; s++)
            {
                buf[s] += v.samples[v.position + s];
                // Soft-clip accumulation
                if (buf[s] >  1.0f) buf[s] =  1.0f;
                if (buf[s] < -1.0f) buf[s] = -1.0f;
            }

            v.position += to_mix;
            if (v.position >= (int)v.samples.size())
                v.active = false;
        }

        SDL_UnlockMutex(_mutex);
    }
}
