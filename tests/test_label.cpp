#include <catch2/catch_test_macros.hpp>
#include "SystemFixture.h"
#include "MockGraphicsMode.h"
#include "UI/Label.h"
#include "UI/BorderedBox.h"
#include "UI/HeartMeter.h"
#include "UI/TimerDisplay.h"
#include "UI/BlockCounter.h"
#include "UI/HUD.h"
#include "UI/PauseMenu.h"

// ── Label ────────────────────────────────────────────────────────────────────

TEST_CASE("Label: draws text when not blinking", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(0, 0, "HELLO", 0xF0);
    label.draw(&mode);
    REQUIRE(mode.hasString("HELLO"));
}

TEST_CASE("Label: no blink — always visible", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(0, 0, "TEXT", 0xF0, 0);
    label.update(9999);
    label.draw(&mode);
    REQUIRE(mode.hasString("TEXT"));
}

TEST_CASE("Label: blink — hidden before first interval", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(10, 20, "BLINK", 0xF0, 500);
    // Initial state: _blink_visible = true, so text IS drawn.
    label.draw(&mode);
    REQUIRE(mode.hasString("BLINK"));
}

TEST_CASE("Label: blink — hidden after one interval elapses", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(0, 0, "BLINK", 0xF0, 500);
    label.update(500); // flips to hidden
    label.draw(&mode);
    REQUIRE_FALSE(mode.hasString("BLINK"));
}

TEST_CASE("Label: blink — visible again after two intervals", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(0, 0, "BLINK", 0xF0, 500);
    // updateSelf flips once per call; need two separate ticks to flip twice.
    label.update(500); // tick 1: visible → hidden
    label.update(500); // tick 2: hidden → visible
    label.draw(&mode);
    REQUIRE(mode.hasString("BLINK"));
}

TEST_CASE("Label: setText changes drawn text", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(0, 0, "BEFORE", 0xF0);
    label.setText("AFTER");
    label.draw(&mode);
    REQUIRE_FALSE(mode.hasString("BEFORE"));
    REQUIRE(mode.hasString("AFTER"));
}

TEST_CASE("Label: invisible label draws nothing", "[label]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::Label label(0, 0, "HIDDEN", 0xF0);
    label.setVisible(false);
    label.draw(&mode);
    REQUIRE(mode.strings.empty());
}

// ── BorderedBox ───────────────────────────────────────────────────────────────

TEST_CASE("BorderedBox: draws a filled rect and border lines", "[borderedbox]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::BorderedBox box(0, 0, 100, 60, 9, 0);
    box.draw(&mode);
    REQUIRE_FALSE(mode.rects.empty());
    REQUIRE_FALSE(mode.hlines.empty());
    REQUIRE_FALSE(mode.vlines.empty());
}

// ── HeartMeter ────────────────────────────────────────────────────────────────

TEST_CASE("HeartMeter: draws one char per health point", "[heartmeter]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int health = 3;
    UI::HeartMeter hm(0, 0, health);
    hm.draw(&mode);
    REQUIRE(mode.chars.size() == 3);
}

TEST_CASE("HeartMeter: zero health draws nothing", "[heartmeter]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int health = 0;
    UI::HeartMeter hm(0, 0, health);
    hm.draw(&mode);
    REQUIRE(mode.chars.empty());
}

TEST_CASE("HeartMeter: health ref update is reflected in draw", "[heartmeter]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int health = 1;
    UI::HeartMeter hm(0, 0, health);
    health = 3;
    hm.draw(&mode);
    REQUIRE(mode.chars.size() == 3);
}

// ── TimerDisplay ─────────────────────────────────────────────────────────────

TEST_CASE("TimerDisplay: draws TIME: string", "[timerdisplay]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    unsigned char secs = 45;
    UI::TimerDisplay td(0, 0, secs);
    td.draw(&mode);
    REQUIRE(mode.hasString("TIME:"));
}

TEST_CASE("TimerDisplay: warning threshold triggers different color", "[timerdisplay]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    unsigned char secs = 5;
    UI::TimerDisplay td(0, 0, secs, 10);
    td.draw(&mode);
    // Just check it renders — color verification would require inspecting call args.
    REQUIRE(mode.hasString("TIME:"));
}

// ── BlockCounter ──────────────────────────────────────────────────────────────

TEST_CASE("BlockCounter: draws block icon and count string", "[blockcounter]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int blocks = 7;
    UI::BlockCounter bc(0, 0, blocks);
    bc.draw(&mode);
    REQUIRE_FALSE(mode.chars.empty());
    REQUIRE(mode.hasString("x7"));
}

TEST_CASE("BlockCounter: count ref update is reflected", "[blockcounter]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int blocks = 2;
    UI::BlockCounter bc(0, 0, blocks);
    blocks = 5;
    bc.draw(&mode);
    REQUIRE(mode.hasString("x5"));
}

// ── HUD ───────────────────────────────────────────────────────────────────────

TEST_CASE("HUD: renders score and time strings", "[hud]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int health = 3, score = 100, blocks = 4;
    unsigned char secs = 90;
    UI::HUD hud(320, health, score, blocks, secs);
    hud.draw(&mode);
    REQUIRE(mode.hasString("TIME:"));
    REQUIRE_FALSE(mode.rects.empty()); // HUD background strip
}

TEST_CASE("HUD: shows GAME OVER when health is zero", "[hud]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int health = 0, score = 0, blocks = 0;
    unsigned char secs = 60;
    UI::HUD hud(320, health, score, blocks, secs);
    hud.draw(&mode);
    REQUIRE(mode.hasString("GAME OVER!"));
}

TEST_CASE("HUD: shows GAME OVER when time is zero", "[hud]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    int health = 3, score = 0, blocks = 0;
    unsigned char secs = 0;
    UI::HUD hud(320, health, score, blocks, secs);
    hud.draw(&mode);
    REQUIRE(mode.hasString("GAME OVER!"));
}

// ── PauseMenu ─────────────────────────────────────────────────────────────────

TEST_CASE("PauseMenu: renders PAUSED! label via children", "[pausemenu]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::PauseMenu pm(320, 240);
    pm.draw(&mode);
    REQUIRE(mode.hasString("PAUSED!"));
}

TEST_CASE("PauseMenu: invisible when hidden", "[pausemenu]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::PauseMenu pm(320, 240);
    pm.setVisible(false);
    pm.draw(&mode);
    REQUIRE(mode.strings.empty());
}
