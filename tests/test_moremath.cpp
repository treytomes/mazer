#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "moremath.h"

TEST_CASE("isInRange: inclusive lower, exclusive upper", "[moremath]")
{
    REQUIRE(isInRange(0.0f,  0.0f, 10.0f) == true);
    REQUIRE(isInRange(9.9f,  0.0f, 10.0f) == true);
    REQUIRE(isInRange(10.0f, 0.0f, 10.0f) == false);
    REQUIRE(isInRange(-0.1f, 0.0f, 10.0f) == false);
}

TEST_CASE("randInt: equal bounds returns lower", "[moremath]")
{
    REQUIRE(randInt(5, 5) == 5);
}

TEST_CASE("randInt: result stays in [min, max)", "[moremath]")
{
    setRandSeed(42);
    for (int i = 0; i < 1000; i++)
    {
        int v = randInt(0, 10);
        REQUIRE(v >= 0);
        REQUIRE(v < 10);
    }
}

TEST_CASE("min/max", "[moremath]")
{
    REQUIRE(min(3, 7) == 3);
    REQUIRE(min(7, 3) == 3);
    REQUIRE(min(-1, 0) == -1);
    REQUIRE(max(3, 7) == 7);
    REQUIRE(max(7, 3) == 7);
    REQUIRE(max(-1, 0) == 0);
}

TEST_CASE("distance: zero distance for same point", "[moremath]")
{
    REQUIRE(distance(5, 5, 5, 5) == Catch::Approx(0.0f));
}

TEST_CASE("distance: 3-4-5 triangle", "[moremath]")
{
    REQUIRE(distance(0, 0, 3, 4) == Catch::Approx(5.0f));
}

TEST_CASE("randBool: produces both true and false", "[moremath]")
{
    setRandSeed(1);
    bool saw_true = false, saw_false = false;
    for (int i = 0; i < 100; i++)
    {
        bool b = randBool();
        if (b)  saw_true  = true;
        if (!b) saw_false = true;
    }
    REQUIRE(saw_true);
    REQUIRE(saw_false);
}
