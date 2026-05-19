#include <catch2/catch_test_macros.hpp>
#include "SystemFixture.h"
#include "MockGraphicsMode.h"
#include "UI/CurtainTransition.h"

static const int SW = 320;
static const int SH = 240;

TEST_CASE("CurtainTransition: starts idle and done", "[curtain]")
{
    SystemFixture sys;
    UI::CurtainTransition c(SW, SH);
    REQUIRE(c.isDone());
}

TEST_CASE("CurtainTransition: draw does nothing when fully open", "[curtain]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::CurtainTransition c(SW, SH);
    c.draw(&mode);
    REQUIRE(mode.rects.empty());
}

TEST_CASE("CurtainTransition: closeScreen makes it not done", "[curtain]")
{
    SystemFixture sys;
    UI::CurtainTransition c(SW, SH);
    c.closeScreen();
    REQUIRE_FALSE(c.isDone());
}

TEST_CASE("CurtainTransition: closeScreen draws panels after update", "[curtain]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::CurtainTransition c(SW, SH);
    c.closeScreen();
    c.update(16);
    c.draw(&mode);
    REQUIRE_FALSE(mode.rects.empty()); // left and right panels
}

TEST_CASE("CurtainTransition: closeScreen reaches idle after enough ticks", "[curtain]")
{
    SystemFixture sys;
    UI::CurtainTransition c(SW, SH);
    c.closeScreen();
    // CURTAIN_SPEED=2 per tick, need SW/2=160 pixels → 80 ticks minimum
    for (int i = 0; i < 100; i++)
        c.update(1);
    REQUIRE(c.isDone());
}

TEST_CASE("CurtainTransition: open after close clears panels", "[curtain]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::CurtainTransition c(SW, SH);
    c.closeScreen();
    for (int i = 0; i < 100; i++) c.update(1);

    c.open();
    REQUIRE_FALSE(c.isDone());
    // Run until done
    for (int i = 0; i < 100; i++) c.update(1);
    REQUIRE(c.isDone());

    mode.clear();
    c.draw(&mode);
    REQUIRE(mode.rects.empty()); // fully open → nothing drawn
}

TEST_CASE("CurtainTransition: two panels drawn symmetrically during close", "[curtain]")
{
    SystemFixture sys;
    MockGraphicsMode mode;
    UI::CurtainTransition c(SW, SH);
    c.closeScreen();
    c.update(10); // advance several pixels
    c.draw(&mode);
    // Expect exactly two rects (left and right panels)
    REQUIRE(mode.rects.size() == 2);
}
