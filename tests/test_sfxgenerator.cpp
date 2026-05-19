#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "AUDIO/SfxGenerator.h"
#include "AUDIO/SfxParams.h"

using namespace AUDIO;

TEST_CASE("SfxGenerator: generates non-empty output", "[sfx]")
{
    SfxParams p = Presets::MenuSelect();
    auto samples = SfxGenerator::generate(p, 44100);
    REQUIRE_FALSE(samples.empty());
}

TEST_CASE("SfxGenerator: all samples are in [-1, 1]", "[sfx]")
{
    SfxParams p = Presets::WhirligigHit();
    auto samples = SfxGenerator::generate(p, 44100);
    for (float s : samples)
    {
        REQUIRE(s >= -1.0f);
        REQUIRE(s <=  1.0f);
    }
}

TEST_CASE("SfxGenerator: duration matches attack+sustain+decay at 44100", "[sfx]")
{
    SfxParams p{};
    p.wave_type    = WAVE_SQUARE;
    p.attack_time  = 0.1f;
    p.sustain_time = 0.2f;
    p.decay_time   = 0.3f;
    p.base_freq    = 0.5f;
    p.volume       = 0.5f;
    p.duty         = 0.5f;
    p.lpf_freq     = 1.0f;
    p.hpf_freq     = 0.0f;

    auto samples = SfxGenerator::generate(p, 44100);
    int expected = (int)((0.1f + 0.2f + 0.3f) * 44100);
    REQUIRE((int)samples.size() == expected);
}

TEST_CASE("SfxGenerator: zero volume produces near-silent output", "[sfx]")
{
    SfxParams p = Presets::MenuNavigate();
    p.volume = 0.0f;
    auto samples = SfxGenerator::generate(p, 44100);
    for (float s : samples)
    {
        REQUIRE(s >= -0.01f);
        REQUIRE(s <=  0.01f);
    }
}

TEST_CASE("SfxGenerator: all wave types produce output", "[sfx]")
{
    int wave_types[] = { WAVE_SQUARE, WAVE_SAW, WAVE_SINE, WAVE_TRIANGLE, WAVE_NOISE };
    for (int wt : wave_types)
    {
        SfxParams p{};
        p.wave_type    = wt;
        p.attack_time  = 0.0f;
        p.sustain_time = 0.1f;
        p.decay_time   = 0.1f;
        p.base_freq    = 0.5f;
        p.volume       = 0.5f;
        p.duty         = 0.5f;
        p.lpf_freq     = 1.0f;
        p.hpf_freq     = 0.0f;

        auto samples = SfxGenerator::generate(p, 44100);
        REQUIRE_FALSE(samples.empty());
    }
}

TEST_CASE("SfxGenerator: presets cycle returns valid preset", "[sfx]")
{
    for (int i = 0; i < Presets::COUNT; i++)
    {
        SfxParams p = Presets::get(i);
        auto samples = SfxGenerator::generate(p, 44100);
        REQUIRE_FALSE(samples.empty());
        // Verify all samples clipped
        for (float s : samples)
        {
            REQUIRE(s >= -1.0f);
            REQUIRE(s <=  1.0f);
        }
    }
}

TEST_CASE("SfxGenerator: preset names are non-empty", "[sfx]")
{
    for (int i = 0; i < Presets::COUNT; i++)
    {
        const char* n = Presets::name(i);
        REQUIRE(n != nullptr);
        REQUIRE(n[0] != '\0');
    }
}

TEST_CASE("SfxGenerator: higher sample rate produces more samples", "[sfx]")
{
    SfxParams p = Presets::PickupFood();
    auto s44 = SfxGenerator::generate(p, 44100);
    auto s22 = SfxGenerator::generate(p, 22050);
    REQUIRE(s44.size() > s22.size());
}
