#include <catch2/catch_test_macros.hpp>
#include "maze.h"

// ── MazeCell ────────────────────────────────────────────────────────────────

TEST_CASE("MazeCell: default state is unvisited, no prize", "[maze][cell]")
{
    MazeCell c;
    REQUIRE_FALSE(c.north_open);
    REQUIRE_FALSE(c.south_open);
    REQUIRE_FALSE(c.east_open);
    REQUIRE_FALSE(c.west_open);
    REQUIRE(c.prize_type == PRIZE_NONE);
    REQUIRE_FALSE(c.isVisited());
}

TEST_CASE("MazeCell: isVisited returns true when any opening set", "[maze][cell]")
{
    MazeCell c;
    c.east_open = true;
    REQUIRE(c.isVisited());
}

TEST_CASE("MazeCell: numOpenings counts correctly", "[maze][cell]")
{
    MazeCell c;
    REQUIRE(c.numOpenings() == 0);
    c.north_open = true;
    REQUIRE(c.numOpenings() == 1);
    c.south_open = true;
    REQUIRE(c.numOpenings() == 2);
    c.east_open = true;
    c.west_open = true;
    REQUIRE(c.numOpenings() == 4);
}

TEST_CASE("MazeCell: isDeadEnd requires exactly one opening", "[maze][cell]")
{
    MazeCell c;
    REQUIRE_FALSE(c.isDeadEnd()); // 0 openings — not a dead end

    c.north_open = true;
    REQUIRE(c.isDeadEnd()); // 1 opening

    c.south_open = true;
    REQUIRE_FALSE(c.isDeadEnd()); // 2 openings
}

// ── Maze class ───────────────────────────────────────────────────────────────

TEST_CASE("Maze: getCell returns null for out-of-bounds", "[maze]")
{
    Maze m;
    REQUIRE(m.getCell(-1, 0)         == nullptr);
    REQUIRE(m.getCell(0, -1)         == nullptr);
    REQUIRE(m.getCell(m.rows, 0)     == nullptr);
    REQUIRE(m.getCell(0, m.columns)  == nullptr);
}

TEST_CASE("Maze: getCell returns valid cell for in-bounds", "[maze]")
{
    Maze m;
    REQUIRE(m.getCell(0, 0)            != nullptr);
    REQUIRE(m.getCell(m.rows-1, m.columns-1) != nullptr);
}

TEST_CASE("Maze: Dijkstra map initialised to -1", "[maze]")
{
    Maze m;
    REQUIRE(m.getDijkstra(0, 0) == -1);
    REQUIRE(m.getDijkstra(m.rows-1, m.columns-1) == -1);
}

TEST_CASE("Maze: getDijkstra returns -255 for out-of-bounds", "[maze]")
{
    Maze m;
    REQUIRE(m.getDijkstra(-1, 0) == -255);
    REQUIRE(m.getDijkstra(0, m.columns) == -255);
}

TEST_CASE("Maze: setDijkstra and clearDijkstra round-trip", "[maze]")
{
    Maze m;
    m.setDijkstra(2, 3, 42);
    REQUIRE(m.getDijkstra(2, 3) == 42);

    m.clearDijkstra();
    REQUIRE(m.getDijkstra(2, 3) == -1);
}

TEST_CASE("Maze: setDijkstra returns false for out-of-bounds", "[maze]")
{
    Maze m;
    REQUIRE_FALSE(m.setDijkstra(-1, 0, 5));
    REQUIRE_FALSE(m.setDijkstra(0, m.columns, 5));
}

TEST_CASE("Maze: start/finish coordinates initialise to 0,0", "[maze]")
{
    Maze m;
    REQUIRE(m.start_row    == 0);
    REQUIRE(m.start_column == 0);
    REQUIRE(m.finish_row   == 0);
    REQUIRE(m.finish_column == 0);
}

// ── generateMaze ─────────────────────────────────────────────────────────────

TEST_CASE("generateMaze: every cell is visited", "[maze][generate]")
{
    setRandSeed(1);
    Maze m;
    generateMaze(&m);

    for (int r = 0; r < m.rows; r++)
        for (int c = 0; c < m.columns; c++)
            REQUIRE(m.getCell(r, c)->isVisited());
}

TEST_CASE("generateMaze: start and finish are marked", "[maze][generate]")
{
    setRandSeed(2);
    Maze m;
    generateMaze(&m);

    REQUIRE(m.getCell(m.start_row, m.start_column)->prize_type == PRIZE_START);
    REQUIRE(m.getCell(m.finish_row, m.finish_column)->prize_type == PRIZE_FINISH);
}

TEST_CASE("generateMaze: start and finish are different cells", "[maze][generate]")
{
    setRandSeed(3);
    Maze m;
    generateMaze(&m);

    REQUIRE((m.start_row != m.finish_row || m.start_column != m.finish_column));
}

TEST_CASE("generateMaze: Dijkstra map is populated after generation", "[maze][generate]")
{
    setRandSeed(4);
    Maze m;
    generateMaze(&m);

    // Every cell should have a non-negative distance from start.
    for (int r = 0; r < m.rows; r++)
        for (int c = 0; c < m.columns; c++)
            REQUIRE(m.getDijkstra(r, c) >= 0);
}

TEST_CASE("generateMaze: neighbour cells share openings consistently", "[maze][generate]")
{
    setRandSeed(5);
    Maze m;
    generateMaze(&m);

    for (int r = 0; r < m.rows; r++)
    {
        for (int c = 0; c < m.columns; c++)
        {
            MazeCell* cell = m.getCell(r, c);
            if (cell->north_open && r > 0)
                REQUIRE(m.getCell(r-1, c)->south_open);
            if (cell->south_open && r < m.rows-1)
                REQUIRE(m.getCell(r+1, c)->north_open);
            if (cell->west_open && c > 0)
                REQUIRE(m.getCell(r, c-1)->east_open);
            if (cell->east_open && c < m.columns-1)
                REQUIRE(m.getCell(r, c+1)->west_open);
        }
    }
}
