#include <catch2/catch_test_macros.hpp>
#include "SystemFixture.h"
#include "MockGraphicsMode.h"
#include "UI/ScoreScreen.h"
#include "KEYBOARD.h"

TEST_CASE("ScoreScreen: not done immediately after construction", "[scorescreen]")
{
    SystemFixture sys;
    UI::ScoreScreen ss(320, 240, 3, 60, 100, true);
    REQUIRE_FALSE(ss.wantsClose());
}

TEST_CASE("ScoreScreen: draws tally lines", "[scorescreen]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::ScoreScreen ss(320, 240, 3, 60, 100, false);
    ss.draw(&mode);
    REQUIRE(mode.hasString("HEARTS"));
    REQUIRE(mode.hasString("TIME"));
    REQUIRE(mode.hasString("SCORE"));
}

TEST_CASE("ScoreScreen: shows finish bonus line when maze finished", "[scorescreen]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::ScoreScreen ss(320, 240, 0, 0, 0, true);
    ss.draw(&mode);
    REQUIRE(mode.hasString("FINISHED MAZE"));
}

TEST_CASE("ScoreScreen: no finish bonus line when maze not finished", "[scorescreen]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    // Drain health/time/score to zero so _finish_bonus check in draw is reached.
    UI::ScoreScreen ss(320, 240, 0, 0, 0, false);
    ss.draw(&mode);
    REQUIRE_FALSE(mode.hasString("FINISHED MAZE"));
}

TEST_CASE("ScoreScreen: tally drives toward done after enough updates", "[scorescreen]")
{
    SystemFixture sys;
    // health=1 → 1 tick at 300ms, time=1 → 1 tick at 80ms,
    // score=1 → 1 tick at 8ms, no finish bonus
    // Then one more tick to reach _tally_done = true.
    UI::ScoreScreen ss(320, 240, 1, 1, 1, false);
    for (int i = 0; i < 50; i++)
        ss.update(300);
    // Should be past all counters now.
    // Not closed yet (waiting for Y/N keypress), but tally should be done.
    REQUIRE_FALSE(ss.wantsClose());
}

TEST_CASE("ScoreScreen: Y scancode causes close with playAgain=true", "[scorescreen]")
{
    SystemFixture sys;
    KEYBOARD::setScanCode(0);
    UI::ScoreScreen ss(320, 240, 0, 0, 0, false);
    for (int i = 0; i < 50; i++)
        ss.update(300);

    KEYBOARD::setScanCode(SDL_SCANCODE_Y);
    ss.update(1);

    REQUIRE(ss.wantsClose());
    REQUIRE(ss.playAgain());
}

TEST_CASE("ScoreScreen: N scancode causes close with playAgain=false", "[scorescreen]")
{
    SystemFixture sys;
    KEYBOARD::setScanCode(0);
    UI::ScoreScreen ss(320, 240, 0, 0, 0, false);
    for (int i = 0; i < 50; i++)
        ss.update(300);

    KEYBOARD::setScanCode(SDL_SCANCODE_N);
    ss.update(1);

    REQUIRE(ss.wantsClose());
    REQUIRE_FALSE(ss.playAgain());
}

TEST_CASE("ScoreScreen: shows PLAY AGAIN after tally and blink on", "[scorescreen]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    KEYBOARD::setScanCode(0);
    // All-zero inputs: tally finishes on the very first tick (consume_speed=300ms).
    UI::ScoreScreen ss(320, 240, 0, 0, 0, false);
    ss.update(300); // _tally_done becomes true; blink not yet started
    ss.update(500); // BLINK_SPEED_MS = 500ms → _blink_visible flips to true
    ss.draw(&mode);
    REQUIRE(mode.hasString("PLAY AGAIN"));
}
