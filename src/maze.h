#pragma once

#include "moremath.h"

#include <cstring>

const int CLOCK_PRIZE_FREQUENCY = 2; // percent
const int CLOCK_PRIZE_VALUE = 10; // seconds
const int CLOCK_PRIZE_SCORE = 10; // points

const int HEALTH_PRIZE_FREQUENCY = 1; // percent
const int HEALTH_PRIZE_VALUE = 1; // hp
const int HEALTH_PRIZE_SCORE = 20; // points

enum PrizeTypes
{
	PRIZE_NONE = 0,
	PRIZE_START = 1,
	PRIZE_FINISH = 2,
	PRIZE_FOOD = 3,
	PRIZE_TIME = 4,
	PRIZE_HEALTH = 5
};

struct MazeCell
{
	bool north_open;
	bool south_open;
	bool east_open;
	bool west_open;
	PrizeTypes prize_type;

	MazeCell()
	{
		north_open = false;
		south_open = false;
		east_open = false;
		west_open = false;
		prize_type = PRIZE_NONE;
	}

	bool isVisited()
	{
		return north_open || south_open || east_open || west_open;
	}

	int numOpenings()
	{
		return
			(north_open ? 1 : 0) +
			(south_open ? 1 : 0) +
			(west_open ? 1 : 0) +
			(east_open ? 1 : 0);
	}

	bool isDeadEnd()
	{
		return numOpenings() == 1;
	}
};

class Maze
{
private:
	const int MAZE_ROWS = 36;
	const int MAZE_COLUMNS = 36;

public:
	const int MAZE_CELL_SIZE = 12;

	int rows;
	int columns;
	MazeCell *cells;
	int *dijkstra_map;

	int start_row;
	int start_column;
	int finish_row;
	int finish_column;

	Maze()
	{
		rows = MAZE_ROWS;
		columns = MAZE_COLUMNS;

		start_row = 0;
		start_column = 0;
		finish_row = 0;
		finish_column = 0;

		cells = new MazeCell[MAZE_ROWS * MAZE_COLUMNS];
		dijkstra_map = new int[MAZE_ROWS * MAZE_COLUMNS];
		clearDijkstra();
	}

	~Maze()
	{
		delete [] cells;
		delete [] dijkstra_map;
	}

	MazeCell *getCell(int row, int column)
	{
		if (!isInRange(row, 0, rows) || !isInRange(column, 0, columns))
		{
			return NULL;
		}
		return &cells[row * columns + column];
	}

	void clearDijkstra()
	{
		memset(dijkstra_map, -1, rows * columns * sizeof(int));
	}

	int getDijkstra(int row, int column)
	{
		if (isInRange(row, 0, rows) && isInRange(column, 0, columns))
		{
			return dijkstra_map[row * columns + column];
		}
		return -255; // out of bounds / unreachable
	}

	bool setDijkstra(int row, int column, int value)
	{
		if (isInRange(row, 0, rows) && isInRange(column, 0, columns))
		{
			dijkstra_map[row * columns + column] = value;
			return true;
		}
		return false;
	}
};

/*
void generateDijkstraMapRecursive(Maze *maze, int current_row, int current_column, int base_score = 0);
void generateDijkstraMap(Maze *maze, int start_row, int start_column);
int calculateFarthestCell(Maze *maze, int &target_row, int &target_column);
void generatePrize(Maze *maze, int row, int column);
void generateEndPoints(Maze *maze);
void generateMazeBinaryTree(Maze *maze);
void generateMazeSidewinder(Maze *maze);
void generateMazeAldousBroder(Maze *maze);
void generateMazeWilson(Maze *maze);
void generateMazeHuntAndKill(Maze *maze);
int countDeadEnds(Maze *maze);
*/

void generateMaze(Maze *maze);
void generateDijkstraMap(Maze *maze, int start_row, int start_column);
