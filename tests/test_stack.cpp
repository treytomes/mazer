#include <catch2/catch_test_macros.hpp>
#include "stack.h"

TEST_CASE("Stack: basic push and pop", "[stack]")
{
    Stack<int> s(4);
    REQUIRE(s.numItems() == 0);

    s.push(10);
    s.push(20);
    REQUIRE(s.numItems() == 2);

    REQUIRE(s.pop() == 20);
    REQUIRE(s.pop() == 10);
    REQUIRE(s.numItems() == 0);
}

TEST_CASE("Stack: pop on empty stack returns default", "[stack]")
{
    Stack<int> s(4);
    int result = s.pop();
    REQUIRE(result == 0);
    REQUIRE(s.numItems() == 0);
}

TEST_CASE("Stack: push beyond capacity is a no-op", "[stack]")
{
    Stack<int> s(2);
    s.push(1);
    s.push(2);
    s.push(3); // should be silently ignored
    REQUIRE(s.numItems() == 2);
    REQUIRE(s.pop() == 2);
    REQUIRE(s.pop() == 1);
}

TEST_CASE("Stack: clear resets count", "[stack]")
{
    Stack<int> s(4);
    s.push(5);
    s.push(6);
    s.clear();
    REQUIRE(s.numItems() == 0);
}

TEST_CASE("Stack: LIFO ordering", "[stack]")
{
    Stack<int> s(5);
    for (int i = 1; i <= 5; i++)
        s.push(i);
    for (int i = 5; i >= 1; i--)
        REQUIRE(s.pop() == i);
}

TEST_CASE("Stack: works with pairs of ints (Dijkstra pattern)", "[stack]")
{
    Stack<int> s(8);
    s.push(3); s.push(7);  // row, col pair
    s.push(0); s.push(0);

    REQUIRE(s.pop() == 0); // col
    REQUIRE(s.pop() == 0); // row
    REQUIRE(s.pop() == 7); // col
    REQUIRE(s.pop() == 3); // row
}
