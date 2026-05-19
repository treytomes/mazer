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
    UI::ScoreScreen ss(320, 240, 0, 0, 0, false);
    ss.draw(&mode);
    REQUIRE_FALSE(mode.hasString("FINISHED MAZE"));
}

TEST_CASE("ScoreScreen: tally drives toward done after enough updates", "[scorescreen]")
{
    SystemFixture sys;
    UI::ScoreScreen ss(320, 240, 1, 1, 1, false);
    for (int i = 0; i < 50; i++)
        ss.update(300);
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
    UI::ScoreScreen ss(320, 240, 0, 0, 0, false);
    ss.update(300); // _tally_done becomes true; blink not yet started
    ss.update(500); // BLINK_SPEED_MS = 500ms → _blink_visible flips to true
    ss.draw(&mode);
    REQUIRE(mode.hasString("PLAY AGAIN"));
}

TEST_CASE("ScoreScreen: speed multiplier 2 drains tally twice as fast", "[scorescreen]")
{
    SystemFixture sys;
    // health=1: normally needs 300ms per tick.
    // At 2x, 150ms of real dt is enough (150 * 2 = 300).
    UI::ScoreScreen fast(320, 240, 1, 0, 0, false);
    fast.setSpeedMultiplier(2);
    fast.update(150); // contributes 300ms effective → health drains
    fast.update(150); // contributes 300ms effective → tally_done

    // Tally should be done; screen waiting for Y/N (not yet closed).
    KEYBOARD::setScanCode(SDL_SCANCODE_Y);
    fast.update(1);
    REQUIRE(fast.wantsClose());
}

TEST_CASE("ScoreScreen: speed multiplier 1 is default (no change in timing)", "[scorescreen]")
{
    SystemFixture sys;
    // health=1: needs one 300ms tick to drain.
    // At 1x, 150ms is not enough.
    UI::ScoreScreen normal(320, 240, 1, 0, 0, false);
    normal.setSpeedMultiplier(1);
    normal.update(150); // only 150ms effective — health not yet drained
    // health still 1, tally not done → not closeable via Y yet
    KEYBOARD::setScanCode(SDL_SCANCODE_Y);
    normal.update(1);
    REQUIRE_FALSE(normal.wantsClose());
}
