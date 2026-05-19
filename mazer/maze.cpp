#include "maze.h"
#include "logging.h"
#include "stack.h"

// Easy to read, but if the map is too big you'll get a stack overflow.
void generateDijkstraMapRecursive(Maze *maze, int current_row, int current_column, int base_score = 0)
{
	if ((current_row < 0) || (current_row >= maze->rows) || (current_column < 0) || (current_column >= maze->columns))
	{
		// The cell is out of bounds.
		return;
	}

	if (maze->getDijkstra(current_row, current_column) != -1)
	{
		// The cell has already been visited.
		return;
	}

	maze->setDijkstra(current_row, current_column, base_score);

	MazeCell *current_cell = maze->getCell(current_row, current_column);
	if (current_cell->north_open)
	{
		generateDijkstraMapRecursive(maze, current_row - 1, current_column, base_score + 1);
	}
	if (current_cell->south_open)
	{
		generateDijkstraMapRecursive(maze, current_row + 1, current_column, base_score + 1);
	}
	if (current_cell->west_open)
	{
		generateDijkstraMapRecursive(maze, current_row, current_column - 1, base_score + 1);
	}
	if (current_cell->east_open)
	{
		generateDijkstraMapRecursive(maze, current_row, current_column + 1, base_score + 1);
	}
}

void generateDijkstraMap(Maze *maze, int start_row, int start_column)
{
	LOG_ENTER();

	maze->clearDijkstra();

	Stack<int> branches(maze->rows * maze->columns * 2);
	int unvisited_cells = maze->rows * maze->columns;

	maze->setDijkstra(start_row, start_column, 0);
	branches.push(start_row);
	branches.push(start_column);
	unvisited_cells--;

	while (branches.numItems() > 0)
	{
		//printf("Branches: %d / %d\n", branch_count, maze->rows * maze->columns);

		int column = branches.pop();
		int row = branches.pop();

		//printf("Calculating branches from (%d, %d).\n", row, column);
		MazeCell *cell = maze->getCell(row, column);
		bool branch_north = cell->north_open && (maze->getDijkstra(row - 1, column) == -1);
		bool branch_south = cell->south_open && (maze->getDijkstra(row + 1, column) == -1);
		bool branch_west = cell->west_open && (maze->getDijkstra(row, column - 1) == -1);
		bool branch_east = cell->east_open && (maze->getDijkstra(row, column + 1) == -1);

		// Now assign the scores, then add each of these to the cell stack.
		int new_score = maze->getDijkstra(row, column) + 1;
		//printf("Assigning score: %d.\n", new_score);
		if (branch_north)
		{
			if (maze->setDijkstra(row - 1, column, new_score))
			{
				branches.push(row - 1);
				branches.push(column);
				unvisited_cells--;
			}
		}
		if (branch_south)
		{
			if (maze->setDijkstra(row + 1, column, new_score))
			{
				branches.push(row + 1);
				branches.push(column);
				unvisited_cells--;
			}
		}
		if (branch_west)
		{
			if (maze->setDijkstra(row, column - 1, new_score))
			{
				branches.push(row);
				branches.push(column - 1);
				unvisited_cells--;
			}
		}
		if (branch_east)
		{
			if (maze->setDijkstra(row, column + 1, new_score))
			{
				branches.push(row);
				branches.push(column + 1);
				unvisited_cells--;
			}
		}
	}

	//printf("Unvisited cells: %d / %d\n", unvisited_cells, maze->rows * maze->columns);

	LOG_LEAVE();
}

int calculateFarthestCell(Maze *maze, int &target_row, int &target_column)
{
	int target_score = maze->getDijkstra(target_row, target_column);
	for (int row = 0; row < maze->rows; row++)
	{
		for (int column = 0; column < maze->columns; column++)
		{
			int new_score = maze->getDijkstra(row, column);
			if (new_score > target_score)
			{
				target_row = row;
				target_column = column;
				target_score = new_score;
			}
		}
	}
	return target_score;
}

void generatePrize(Maze *maze, int row, int column)
{
	//LOG_ENTER();

	MazeCell *cell = maze->getCell(row, column);
	int chance = randInt(0, 100);
	if (chance >= (100 - CLOCK_PRIZE_FREQUENCY))
	{
		maze->getCell(row, column)->prize_type = PRIZE_TIME;
	}
	else if (chance >= (100 - CLOCK_PRIZE_FREQUENCY - HEALTH_PRIZE_FREQUENCY))
	{
		maze->getCell(row, column)->prize_type = PRIZE_HEALTH;
	}
	else
	{
		maze->getCell(row, column)->prize_type = PRIZE_FOOD;
	}

	//LOG_LEAVE();
}

void generateEndPoints(Maze *maze)
{
	LOG_ENTER();

	LOG("Generating Dijkstra map - 1.");
	generateDijkstraMap(maze, 0, 0);

	calculateFarthestCell(maze, maze->finish_row, maze->finish_column);

	LOG("Generating Dijkstra map - 2.");
	generateDijkstraMap(maze, maze->finish_row, maze->finish_column);

	// Start cell is at one end of the longest path.
	calculateFarthestCell(maze, maze->start_row, maze->start_column);

	// Reset the dijkstra map with the new starting cell.
	LOG("Generating Dijkstra map - 3.");
	generateDijkstraMap(maze, maze->start_row, maze->start_column);

	maze->getCell(maze->start_row, maze->start_column)->prize_type = PRIZE_START;
	maze->getCell(maze->finish_row, maze->finish_column)->prize_type = PRIZE_FINISH;

	LOG_LEAVE();
}

// Generate the maze using the Binary Tree Algorithm.
void generateMazeBinaryTree(Maze *maze)
{
	for (int row = 0; row < maze->rows; row++)
	{
		for (int column = 0; column < maze->columns; column++)
		{
			if ((row == 0) && column == (maze->columns - 1))
			{
				continue;
			}

			bool goNorth = randBool();
			if (row == 0)
			{
				goNorth = false;
			}
			else if (column == maze->columns - 1)
			{
				goNorth = true;
			}

			if (goNorth)
			{
				maze->getCell(row, column)->north_open = true;
				if ((row - 1) >= 0)
				{
					maze->getCell(row - 1, column)->south_open = true;
				}
			}
			else
			{
				maze->getCell(row, column)->east_open = true;
				if ((column + 1) < maze->columns)
				{
					maze->getCell(row, column + 1)->west_open = true;
				}
			}
		}
	}
}

void generateMazeSidewinder(Maze *maze)
{
	for (int row = 0; row < maze->rows; row++)
	{
		int run_start = 0;
		for (int column = 0; column < maze->columns; column++)
		{
			bool choice = randBool();
			if (row == 0)
			{
				if (column == maze->columns - 1)
				{
					continue;
				}
				choice = true;
			}
			else if (column == maze->columns - 1)
			{
				choice = false;
			}

			if (choice)
			{
				// Go east.
				maze->getCell(row, column)->east_open = true;
				maze->getCell(row, column + 1)->west_open = true;
			}
			else
			{
				// Tunnel north to complete the current run.
				int northCell = randInt(run_start, column);
				maze->getCell(row, northCell)->north_open = true;
				maze->getCell(row - 1, northCell)->south_open = true;
				run_start = column + 1;
			}
		}
	}
}

// The Random Walk Algorithm
void generateMazeAldousBroder(Maze *maze)
{
	int num_unvisited_cells = maze->rows * maze->columns;

	int current_row = randInt(0, maze->rows);
	int current_column = randInt(0, maze->columns);
	num_unvisited_cells--;

	while (num_unvisited_cells > 0)
	{
		MazeCell *current_cell = maze->getCell(current_row, current_column);
	
		// Pick a direction.
		int new_row = current_row;
		int new_column = current_column;

		bool done = false;
		int direction = 0;
		while (!done)
		{
			direction = randInt(0, 4);
			switch (direction)
			{
				case 0:
					if ((new_row - 1) >= 0)
					{
						new_row--;
						done = true;
					}
					break;
				case 1:
					if ((new_row + 1) < maze->rows)
					{
						new_row++;
						done = true;
					}
					break;
				case 2:
					if ((new_column - 1) >= 0)
					{
						new_column--;
						done = true;
					}
					break;
				case 3:
					if ((new_column + 1) < maze->columns)
					{
						new_column++;
						done = true;
					}
					break;
			}
		}
		MazeCell *new_cell = maze->getCell(new_row, new_column);
		bool visited = new_cell->north_open || new_cell->south_open || new_cell->east_open || new_cell->west_open;
		if (!visited)
		{
			switch (direction)
			{
				case 0:
					current_cell->north_open = true;
					new_cell->south_open = true;
					break;
				case 1:
					current_cell->south_open = true;
					new_cell->north_open = true;
					break;
				case 2:
					current_cell->west_open = true;
					new_cell->east_open = true;
					break;
				case 3:
					current_cell->east_open = true;
					new_cell->west_open = true;
					break;
			}
			num_unvisited_cells--;
		}

		current_row = new_row;
		current_column = new_column;
	}
}

// Use Wilson's random-walk/loop-cutting algorithm to generate a maze.
void generateMazeWilson(Maze *maze)
{
	// TODO: Replace path_row/path_column/current_path_length with a Stack<int>.

	int max_path_length = maze->rows * maze->columns;
	int *path_row = new int[max_path_length];
	int *path_column = new int[max_path_length];
	int current_path_length = 0;
	//Stack<int> path(maze->rows * maze->columns);

	bool *visited_cells = new bool[maze->rows * maze->columns];
	memset(visited_cells, false, maze->rows * maze->columns * sizeof(bool));

	// Pick a random cell and mark it as visited.
	int current_row = randInt(0, maze->rows);
	int current_column = randInt(0, maze->columns);
	visited_cells[current_row * maze->columns + current_column] = true;
	int remaining_visited_cells = maze->rows * maze->columns - 1;

	while (remaining_visited_cells > 0)
	{
		// Start a new path.
		current_path_length = 0;
		//path.clear();

		// Pick an unvisited cell for the starting point of the path.
		current_row = randInt(0, maze->rows);
		current_column = randInt(0, maze->columns);
		while (visited_cells[current_row * maze->columns + current_column])
		{
			current_row = randInt(0, maze->rows);
			current_column = randInt(0, maze->columns);
		}
		path_row[current_path_length] = current_row;
		path_column[current_path_length] = current_column;
		current_path_length++;
		//path.push(current_row);
		//path.push(current_column);

		// Walk randomly until we hit a visited cell.
		while (true)
		{
			// Pick a random direction.
			bool direction_picked = false;
			while (!direction_picked)
			{
				// TODO: I could make this more efficient by not picking the same random direction over and over again.
				int direction = randInt(0, 4);
				switch (direction)
				{
				case 0:
					if (current_row > 0)
					{
						current_row--;
						direction_picked = true;
					}
					break;
				case 1:
					if (current_row < maze->rows - 1)
					{
						current_row++;
						direction_picked = true;
					}
					break;
				case 2:
					if (current_column > 0)
					{
						current_column--;
						direction_picked = true;
					}
					break;
				case 3:
					if (current_column < maze->columns - 1)
					{
						current_column++;
						direction_picked = true;
					}
					break;
				}
			}

			// Add the new position to the path.
			path_row[current_path_length] = current_row;
			path_column[current_path_length] = current_column;
			current_path_length++;
			//path.push(current_row);
			//path.push(current_column);

			// If we hit a previously visited cell then we're done.
			if (visited_cells[current_row * maze->columns + current_column])
			{
				// This is the end of the path.
				break;
			}

			// If the new (current_row, current_column) is aleady on the path, then clip the path.
			if (current_path_length > 2) // Has to be longer than 2 to double-back on itself.
			{
				for (int index = 0; index < current_path_length - 1; index++)
				{
					if ((current_row == path_row[index]) && (current_column == path_column[index]))
					{
						// Our path has a loop, so clip it off.
						current_path_length = index + 1;
						break;
					}
				}
			}
		}

		// Build the path into the map and mark it as visited.
		for (int index = 0; index < current_path_length - 1; index++)
		{
			MazeCell *this_cell = maze->getCell(path_row[index], path_column[index]);
			MazeCell *next_cell = maze->getCell(path_row[index + 1], path_column[index + 1]);

			// In which direction is this current cell connected to the next cell?
			if (path_row[index] + 1 == path_row[index + 1])
			{
				// Go south.
				this_cell->south_open = true;
				next_cell->north_open = true;
			}
			else if (path_row[index] - 1 == path_row[index + 1])
			{
				// Go north.
				this_cell->north_open = true;
				next_cell->south_open = true;
			}
			else if (path_column[index] + 1 == path_column[index + 1])
			{
				// Go east.
				this_cell->east_open = true;
				next_cell->west_open = true;
			}
			else if (path_column[index] - 1 == path_column[index + 1])
			{
				// Go west.
				this_cell->west_open = true;
				next_cell->east_open = true;
			}

			visited_cells[path_row[index] * maze->columns + path_column[index]] = true;
			remaining_visited_cells--;
		}
	}

	delete[] visited_cells;
	delete[] path_column;
	delete[] path_row;
}

void generateMazeHuntAndKill(Maze *maze)
{
	LOG_ENTER();

	int num_unvisited_cells = maze->rows * maze->columns;

	int current_row = randInt(0, maze->rows);
	int current_column = randInt(0, maze->columns);
	num_unvisited_cells--;

	MazeCell *previous_cell = NULL;

	while (num_unvisited_cells > 0)
	{
		MazeCell *cell = maze->getCell(current_row, current_column);

		LOG("Walking in a random direction.");

		// Pick a random direction.
		bool direction_picked = false;
		int direction = randInt(0, 4);
		for (int n = 0; (n < 4) && !direction_picked; n++)
		{
			direction = (direction + 1) % 4;

			int new_row = current_row;
			int new_column = current_column;

			if		((direction == 0) && (new_row > 0))						new_row--;
			else if ((direction == 1) && (new_row < maze->rows - 1))		new_row++;
			else if ((direction == 2) && (new_column > 0))					new_column--;
			else if ((direction == 3) && (new_column < maze->columns - 1))	new_column++;
			else continue;

			MazeCell *new_cell = maze->getCell(new_row, new_column);

			if (((previous_cell != NULL) && (new_cell == previous_cell)) || (new_cell == cell))
			{
				continue;
			}

			bool is_visited = new_cell->north_open || new_cell->south_open || new_cell->east_open || new_cell->west_open;
			if (is_visited)
			{
				continue;
			}

			// We haven't been here before, so carve a tunnel and scoot over.

			if (direction == 0)
			{
				cell->north_open = true;
				new_cell->south_open = true;
			}
			else if (direction == 1)
			{
				cell->south_open = true;
				new_cell->north_open = true;
			}
			else if (direction == 2)
			{
				cell->west_open = true;
				new_cell->east_open = true;
			}
			else if (direction == 3)
			{
				cell->east_open = true;
				new_cell->west_open = true;
			}

			current_row = new_row;
			current_column = new_column;
			direction_picked = true;
			num_unvisited_cells--;
		}

		if (!direction_picked)
		{
			LOG("Hunting for a new spot.");

			// We painted ourselves into a corner, so let's hunt for a new spot to carve a tunnel.
			for (int hunt_row = 0; (hunt_row < maze->rows) && !direction_picked; hunt_row++)
			{
				for (int hunt_column = 0; (hunt_column < maze->columns) && !direction_picked; hunt_column++)
				{
					MazeCell *hunt_cell = maze->getCell(hunt_row, hunt_column);
					bool is_visited = hunt_cell->north_open || hunt_cell->south_open || hunt_cell->east_open || hunt_cell->west_open;
					if (is_visited)
					{
						continue;
					}

					// Consider the 4 adjacent cells.
					for (int s_row = max(hunt_row - 1, 0); (s_row <= min(hunt_row + 1, maze->rows - 1)) && !direction_picked; s_row++)
					{
						for (int s_column = max(hunt_column - 1, 0); (s_column <= min(hunt_column + 1, maze->columns - 1)) && !direction_picked; s_column++)
						{
							if ((hunt_row != s_row) && (hunt_column != s_column))
							{
								continue;
							}

							MazeCell *s_cell = maze->getCell(s_row, s_column);
							if (!s_cell->isVisited())
							{
								continue;
							}

							if (s_row == hunt_row - 1)
							{
								hunt_cell->north_open = true;
								s_cell->south_open = true;
							}
							else if (s_row == hunt_row + 1)
							{
								hunt_cell->south_open = true;
								s_cell->north_open = true;
							}
							else if (s_column == hunt_column - 1)
							{
								hunt_cell->west_open = true;
								s_cell->east_open = true;
							}
							else if (s_column == hunt_column + 1)
							{
								hunt_cell->east_open = true;
								s_cell->west_open = true;
							}

							current_row = hunt_row;
							current_column = hunt_column;
							direction_picked = true;
							num_unvisited_cells--;
						}
					}
				}
			}
		}

		previous_cell = cell;
	}

	LOG_LEAVE();
}

int countDeadEnds(Maze *maze)
{
	int count = 0;
	for (int row = 0; row < maze->rows; row++)
	{
		for (int column = 0; column < maze->columns; column++)
		{
			MazeCell *cell = maze->getCell(row, column);
			count += cell->isDeadEnd() ? 1 : 0;
		}
	}
	return count;
}

void generateMaze(Maze *maze)
{
	LOG_ENTER();

	//generateMazeBinaryTree(maze);
	//generateMazeSidewinder(maze);
	//generateMazeAldousBroder(maze);
	
	generateMazeWilson(maze);

	// Generates longer passages with fewer forks.  Makes it harder to avoid whirligigs.  It might work if the player had a way to fight back.
	//generateMazeHuntAndKill(maze);

	for (int row = 0; row < maze->rows; row++)
	{
		for (int column = 0; column < maze->columns; column++)
		{
			generatePrize(maze, row, column);
		}
	}
	generateEndPoints(maze);

	int num_dead_ends = countDeadEnds(maze);
	printf("Generated a maze with %d dead ends (%0.0f%%).\n", num_dead_ends, num_dead_ends / ((float)maze->rows * (float)maze->columns) * 100);

	LOG_LEAVE();
}
