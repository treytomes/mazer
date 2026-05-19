/*

You start with 30 seconds to complete the maze.  Picking up clocks will add 10 seconds per clock.

You also start with 3 hearts, and lose a heart if you collide with a whirligig.  You can get more hearts by picking them up around the maze.

If you run out of hearts or time then the game is over.

You can place blocks by holding left-shift or right-shift + an arrow key.  Blocks will keep whirligigs (or yourself) from passing through,
and last for 10 seconds.

*/

// Block progression: 219 --> 178 --> 177 --> 176

#include "mazer.h"
#include "moremath.h"
#include "maze.h"
#include "stack.h"
#include "logging.h"

// TODO: Time should be worth more than pellets at the final scoring.
// TODO: Whirligigs should be slower and smarter.  Make them follow a wall, or stay within 15 spaces of an area.

// Should the maze solution be drawn?
#define ALLOW_SHOW_SOLUTION
//#define ALLOW_GHOSTING

int maze_seed = 1;

/*
 * System variables.
 */

const int TARGET_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / TARGET_FPS;
const char WINDOW_TITLE[] = "mazer";
Uint32 num_frames = 0;

#include "bitIcons.h"

/*
 * Maze variables.
 */

const int TIME_VALUE = 2; // 2 points per second left on the clock

class Entity
{
public:
	int x;
	int y;
	int x_delta;
	int y_delta;
	int x_target;
	int y_target;
	int size;
	bool is_alive;

	Entity()
		: x(0), y(0), x_delta(0), y_delta(0), x_target(0), y_target(0), size(4), is_alive(false)
	{
	}

	void jumpTo(int _x, int _y)
	{
		x = _x;
		y = _y;
		x_target = _x;
		y_target = _y;
	}
};

const int INITIAL_HEALTH = 3;
class PlayerEntity : public Entity
{
private:
	const int PLAYER_SIZE = 4;
	const int PLAYER_COLOR = 12;

public:
	int score;
	int health;

	PlayerEntity()
		: Entity(), score(0), health(INITIAL_HEALTH)
	{
		size = PLAYER_SIZE;
	}

	void draw(CGA::GraphicsMode *mode)
	{
		int radius = size >> 1;
		mode->drawCircle(mode->getScreenWidth() / 2 + radius, mode->getScreenHeight() / 2 + radius, radius, PLAYER_COLOR);
	}
};

class BlockEntity : public Entity
{
public:
	int birth_time;

	BlockEntity()
		: Entity(), birth_time(0)
	{
		size = 8;
	}
};

Maze *maze = NULL;

/*
 * Time variables.
 */

const int TIME_SPEED_MS = 1000;
unsigned int clock_timer = 0; // Controls time updates and animations. 

// You get ___ seconds to solve the maze.  Maybe you'll find a way to increase the time somewhere.
const char INITIAL_SECONDS = 120;
unsigned char seconds;

/*
 * Player variables.
 */

PlayerEntity player;

bool is_paused = false;

#ifdef ALLOW_SHOW_SOLUTION
bool show_solution = false;
#endif

bool finished_maze;

/*
 * Whirligigs.
 */

const int MAX_WHIRLIGIGS = 16;
Entity whirligigs[MAX_WHIRLIGIGS];

/*
 * Blocks.
 */

const int MAX_BLOCKS = 1;
const int BLOCK_MAX_AGE_MS = 10000;
BlockEntity blocks[MAX_BLOCKS];
int remaining_blocks = MAX_BLOCKS;
bool last_frame_placed_block = false;

/*
 * Effects
 */

const int FLASH_LENGTH_MS = 1000;
// Set to SDL_GetTicks() + FLASH_LENGTH_MS to make the screen flash.
int flash_end_time = 0;

/*
 * End of variables.
 */

void drawHUD(CGA::GraphicsMode *mode)
{
	char buffer[50];

	// The HUD takes up the top 8 pixels of the screen.
	mode->drawFilledRect(0, mode->getScreenWidth() - 1, 0, 7, 0);

	// Calculate and render FPS in a single go.
	num_frames++;
	Uint32 time_secs = (SDL_GetTicks() / 1000);
	if (time_secs > 0)
	{
		sprintf(buffer, "%s, FPS: %d", WINDOW_TITLE, num_frames / time_secs);
		SDL_SetWindowTitle(SYSTEM::window, buffer);
	}

	int x = mode->getScreenWidth();

	// Time
	sprintf(buffer, "TIME:%03d", seconds);
	x -= (strlen(buffer) * 8);
	mode->drawString(x, 0, (seconds <= 10) ? 0xF4 : 0xE0, buffer);

	// Blocks
	sprintf(buffer, "x%01d", remaining_blocks);
	x -= (strlen(buffer) * 8 + 8);
	mode->drawString(x, 0, 0xE0, buffer);

	x -= 1 * 8;
	mode->drawChar(x, 0, 0x60, 254); // small block tile

	// Score
	sprintf(buffer, "%05d", player.score);
	x -= (strlen(buffer) * 8 + 8);
	mode->drawString(x, 0, 0xE0, buffer);

	// Health
	if ((player.health <= 0) || (seconds <= 0))
	{
		// TODO: Did player health go from 2 to game over?
		mode->drawString(0, 0, randInt(1, 16), "GAME OVER!");
	}
	else
	{
		for (unsigned char n = 0; n < player.health; n++)
		{
			mode->drawChar(n * 8, 0, 0x40, 3);
		}
	}
}

void updateTime()
{
	if (SDL_GetTicks() - clock_timer > TIME_SPEED_MS)
	{
		clock_timer = SDL_GetTicks();

		if (seconds > 0)
		{
			seconds--;
		}
	}
}

void tryPlaceBlock(int x_delta, int y_delta)
{
	int block_index = 0;
	for (; (block_index < MAX_BLOCKS) && blocks[block_index].is_alive; block_index++);
	if (block_index >= MAX_BLOCKS)
	{
		return;
	}

	int player_row = player.y / maze->MAZE_CELL_SIZE;
	int player_column = player.x / maze->MAZE_CELL_SIZE;
	MazeCell *player_cell = maze->getCell(player_row, player_column);
	
	int block_row = player_row;
	int block_column = player_column;

	if ((y_delta < 0) && player_cell->north_open)
	{
		block_row--;
	}
	else if ((y_delta > 0) && player_cell->south_open)
	{
		block_row++;
	}
	else if ((x_delta < 0) && player_cell->west_open)
	{
		block_column--;
	}
	else if ((x_delta > 0) && player_cell->east_open)
	{
		block_column++;
	}

	if ((player_row == block_row) && (player_column == block_column))
	{
		return;
	}

	BlockEntity *block = &blocks[block_index];
	block->is_alive = true;
	block->birth_time = SDL_GetTicks();
	block->x = block_column * maze->MAZE_CELL_SIZE;
	block->y = block_row * maze->MAZE_CELL_SIZE;
	last_frame_placed_block = true;
	remaining_blocks--;
}

void updatePlayerEvents() //SDL_Event &e)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	const int cell_size = maze->MAZE_CELL_SIZE;

	bool in_motion = ((player.x != player.x_target) || (player.y != player.y_target));
	if (in_motion)
	{
		//bool moving_north = player.y_target < player.y;
		//bool moving_south = player.y_target > player.y;
		//bool moving_west = player.x_target < player.x;
		//bool moving_east = player.x_target > player.x;

		/*
		bool moving_north = ((player.y_target - player.y) / (float)cell_size) <= -0.1f;
		bool moving_south = ((player.y_target - player.y) / (float)cell_size) >= 0.1f;
		bool moving_west = ((player.x_target - player.x) / (float)cell_size) <= -0.1f;
		bool moving_east = ((player.x_target - player.x) / (float)cell_size) >= 0.1f;

		if (moving_north && !state[SDL_SCANCODE_UP])
		{
			player.y_target = player.y = player.y_target + cell_size;
		}
		if (moving_south && !state[SDL_SCANCODE_DOWN])
		{
			player.y_target = player.y = player.y_target - cell_size;
		}
		if (moving_west && !state[SDL_SCANCODE_LEFT])
		{
			player.x_target = player.x = player.x_target + cell_size;
		}
		if (moving_east && !state[SDL_SCANCODE_RIGHT])
		{
			player.x_target = player.x = player.x_target + cell_size;
		}
		*/

		// Don't accept a new movement command while the player is already in motion.
		return;
	}

	// TODO: If the player is in motion, and the key is released, move backward to the last position.

	MazeCell *cell = maze->getCell(player.y / maze->MAZE_CELL_SIZE, player.x / maze->MAZE_CELL_SIZE);

	bool should_place_block = state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT];
	if (last_frame_placed_block)
	{
		if (should_place_block)
		{
			should_place_block = false;
		}
		else
		{
			last_frame_placed_block = false;
		}
	}

	if (state[SDL_SCANCODE_UP])
	{
		if (should_place_block)
		{
			tryPlaceBlock(0, -1);
		}
		else
		{
#ifndef ALLOW_GHOSTING
			if (cell->north_open)
#endif
			player.y_target = player.y - cell_size;
		}
	}
	else if (state[SDL_SCANCODE_DOWN])
	{
		if (should_place_block)
		{
			tryPlaceBlock(0, 1);
		}
		else
		{
#ifndef ALLOW_GHOSTING
			if (cell->south_open)
#endif
			player.y_target = player.y + cell_size;
		}
	}
	else if (state[SDL_SCANCODE_LEFT])
	{
		if (should_place_block)
		{
			tryPlaceBlock(-1, 0);
		}
		else
		{
#ifndef ALLOW_GHOSTING
			if (cell->west_open)
#endif
			player.x_target = player.x - cell_size;
		}
	}
	else if (state[SDL_SCANCODE_RIGHT])
	{
		if (should_place_block)
		{
			tryPlaceBlock(1, 0);
		}
		else
		{
#ifndef ALLOW_GHOSTING
			if (cell->east_open)
#endif
			player.x_target = player.x + cell_size;
		}
	}
}

// Return true if the move was successful.
bool updateEntity(Entity *entity)
{
	float cell_x = entity->x / (float)maze->MAZE_CELL_SIZE;
	float cell_y = entity->y / (float)maze->MAZE_CELL_SIZE;
	MazeCell *cell = maze->getCell((int)cell_y, (int)cell_x);
	
	entity->x += entity->x_delta;
	entity->y += entity->y_delta;

	float new_cell_x = entity->x / (float)maze->MAZE_CELL_SIZE;
	float new_cell_y = entity->y / (float)maze->MAZE_CELL_SIZE;
	bool new_cell_exists = false;
	if (isInRange(new_cell_y, 0, maze->rows) && isInRange(new_cell_x, 0, maze->columns))
	{
		new_cell_exists = true;
	}

	bool undo_x = !new_cell_exists;
	bool undo_y = undo_x;

	int top = (entity->y - 1) / maze->MAZE_CELL_SIZE;
	int bottom = (entity->y + 2 + entity->size - 1) / maze->MAZE_CELL_SIZE;
	int left = (entity->x - 1) / maze->MAZE_CELL_SIZE;
	int right = (entity->x + 2 + entity->size - 1) / maze->MAZE_CELL_SIZE;

	// Check for blocks.
	bool contains_block = false;
	for (int n = 0; n < MAX_BLOCKS; n++)
	{
		if (!blocks[n].is_alive)
		{
			continue;
		}
		
		int block_row = blocks[n].y / maze->MAZE_CELL_SIZE;
		int block_column = blocks[n].x / maze->MAZE_CELL_SIZE;

		if (((left == block_column) && (top == block_row)) ||
			((left == block_column) && (bottom == block_row)) ||
			((right == block_column) && (top == block_row)) ||
			((right == block_column) && (bottom == block_row)))
		{
			undo_x = true;
			undo_y = true;
			contains_block = true;
		}
	}

	if (new_cell_exists && !contains_block)
	{
		MazeCell *top_left = maze->getCell(top, left);
		MazeCell *top_right = maze->getCell(top, right);
		MazeCell *bottom_left = maze->getCell(bottom, left);
		MazeCell *bottom_right = maze->getCell(bottom, right);

		if ((entity->y - 1) / (float)maze->MAZE_CELL_SIZE < 0)
		{
			undo_y = true;
		}
		else if (top != bottom)
		{
			if (top_left == NULL || top_right == NULL || !top_left->south_open || !top_right->south_open)
			{
				undo_y = true;
			}
			if ((left != right) && (top_left == NULL || bottom_left == NULL || !top_left->east_open || !bottom_left->east_open))
			{
				undo_y = true;
			}
		}

		if ((entity->x - 1) / (float)maze->MAZE_CELL_SIZE < 0)
		{
			undo_x = true;
		}
		else if (left != right)
		{
			if (top_left == NULL || bottom_left == NULL || !top_left->east_open || !bottom_left->east_open)
			{
				undo_x = true;
			}
			if ((top != bottom) && (top_left == NULL || top_right == NULL || !top_left->south_open || !top_right->south_open))
			{
				undo_x = true;
			}
		}
	}

	if (undo_x)
	{
		entity->x -= entity->x_delta;
	}
	if (undo_y)
	{
		entity->y -= entity->y_delta;
	}
	
	return !undo_x && !undo_y;
}

void updatePlayer()
{
	if ((player.health <= 0) || (seconds <= 0))
	{
		player.is_alive = false;
	}

	player.x_delta = 0;
	player.y_delta = 0;
	if (player.x_target < player.x)
	{
		player.x_delta = -1;
	}
	else if (player.x_target > player.x)
	{
		player.x_delta = 1;
	}
	else if (player.y_target < player.y)
	{
		player.y_delta = -1;
	}
	else if (player.y_target > player.y)
	{
		player.y_delta = 1;
	}

#ifdef ALLOW_GHOSTING
	player.x += player.x_delta;
	player.y += player.y_delta;
#else
	if (updateEntity(&player))
#endif
	{
		float cell_x = player.x / (float)maze->MAZE_CELL_SIZE;
		float cell_y = player.y / (float)maze->MAZE_CELL_SIZE;
		MazeCell *cell = maze->getCell((int)cell_y, (int)cell_x);

		// Just moving through the cell.  Grab the prize.
		if (cell != NULL)
		{
			switch (cell->prize_type)
			{
				case PRIZE_FOOD:
					player.score += 1;
					cell->prize_type = PRIZE_NONE;
					break;
				case PRIZE_TIME:
					seconds += CLOCK_PRIZE_VALUE;
					player.score += CLOCK_PRIZE_SCORE;
					cell->prize_type = PRIZE_NONE;
					break;
				case PRIZE_HEALTH:
					player.health += HEALTH_PRIZE_VALUE;
					player.score += HEALTH_PRIZE_SCORE;
					cell->prize_type = PRIZE_NONE;
					break;
				case PRIZE_FINISH:
					finished_maze = true;
					break;
			}
		}
	}
	else
	{
		player.x_target = player.x = (player.x / maze->MAZE_CELL_SIZE) * maze->MAZE_CELL_SIZE + 3;
		player.y_target = player.y = (player.y / maze->MAZE_CELL_SIZE) * maze->MAZE_CELL_SIZE + 3;
	}

	// Check for whirligigs.
	for (int index = 0; index < MAX_WHIRLIGIGS; index++)
	{
		Entity *w = &whirligigs[index];
		if (!w->is_alive)
		{
			continue;
		}

		int dist_x = w->x - player.x;
		int dist_y = w->y - player.y;
		if ((dist_x * dist_x + dist_y * dist_y) <= ((w->size / 2 + player.size / 2) * (w->size / 2 + player.size / 2)))
		{
			player.health--;
			w->is_alive = false;
			flash_end_time = SDL_GetTicks() + FLASH_LENGTH_MS;
			break;
		}
	}
}

void spawnWhirligig(Entity *w)
{
	// Newborn whirligigs must be at least this many cells from the player.
	const int SPAWN_DISTANCE = maze->MAZE_CELL_SIZE * 4;

	do
	{
		w->jumpTo((rand() % maze->columns) * maze->MAZE_CELL_SIZE, (rand() % maze->rows) * maze->MAZE_CELL_SIZE);
	} while (distance(w->x, w->y, player.x, player.y) < SPAWN_DISTANCE);

	w->size = 8;
	w->is_alive = true;

	w->x_delta = 0;
	w->y_delta = 0;

	int row = w->y / maze->MAZE_CELL_SIZE;
	int column = w->x / maze->MAZE_CELL_SIZE;
	MazeCell *cell = maze->getCell(row, column);
	do
	{
		switch (randInt(0, 4))
		{
		case 0:
			if (cell->north_open) w->y_target = w->y - maze->MAZE_CELL_SIZE;
			break;
		case 1:
			if (cell->south_open) w->y_target = w->y + maze->MAZE_CELL_SIZE;
			break;
		case 2:
			if (cell->west_open) w->x_target = w->x - maze->MAZE_CELL_SIZE;
			break;
		case 3:
			if (cell->east_open) w->x_target = w->x + maze->MAZE_CELL_SIZE;
			break;
		}
	} while ((w->x == w->x_target) && (w->y == w->y_target));
}

void updateWhirligigRandom(Entity *w)
{
	if (!updateEntity(w))
	{
		w->x_delta = 0;
		w->y_delta = 0;
		switch (randInt(0, 4))
		{
		case 0:
			w->x_delta = -1;
			break;
		case 1:
			w->x_delta = 1;
			break;
		case 2:
			w->y_delta = -1;
			break;
		case 3:
			w->y_delta = 1;
			break;
		}
	}
}

void updateWhirligigGoRight(Entity *w)
{
	w->x_delta = 0;
	w->y_delta = 0;
	if (w->x_target < w->x)
	{
		w->x_delta = -1;
	}
	else if (w->x_target > w->x)
	{
		w->x_delta = 1;
	}
	else if (w->y_target < w->y)
	{
		w->y_delta = -1;
	}
	else if (w->y_target > w->y)
	{
		w->y_delta = 1;
	}

	bool did_move = updateEntity(w);
	if (!did_move)
	{
		// The way is shut.  Need a new direction.
		w->x_target += w->x_delta * maze->MAZE_CELL_SIZE;
		w->y_target += w->y_delta * maze->MAZE_CELL_SIZE;
		w->x = w->x_target;
		w->y = w->y_target;
	}

	printf("in\n");
	// Keep going if you're able.
	bool is_moving = ((w->x != w->x_target) || (w->y != w->y_target));
	if (is_moving)
	{
		// Still have a ways to go.
		return;
	}
	// No longer moving, so pick a new direction.
	
	// Which direction were you trying to go?
	bool going_north = w->y_delta < 0;
	bool going_south = w->y_delta > 0;
	bool going_west = w->x_delta < 0;
	bool going_east = w->x_delta > 0;

	int row = w->y / maze->MAZE_CELL_SIZE;
	int column = w->x / maze->MAZE_CELL_SIZE;

	if (row < 0) row = 0;
	if (row >= maze->rows) row = maze->rows - 1;
	if (column < 0) column = 0;
	if (column >= maze->columns) column = maze->columns - 1;

	MazeCell *cell = maze->getCell(row, column);
	if (cell == NULL)
	{
		printf("null: %d, %d\n", row, column);
	}
	else
	{
		printf("not null\n");
	}
	printf("out\n");

	if (going_north)
	{
		if (cell->east_open)
		{
			w->y_target = w->y;
			w->x_target = w->x + maze->MAZE_CELL_SIZE;
		}
		else if (!cell->north_open)
		{
			w->y_target = w->y + maze->MAZE_CELL_SIZE;
		}
		else
		{
			w->y_target = w->y - maze->MAZE_CELL_SIZE;
		}
	}
	else if (going_south)
	{
		if (cell->west_open)
		{
			w->y_target = w->y;
			w->x_target = w->x - maze->MAZE_CELL_SIZE;
		}
		else if (!cell->south_open)
		{
			w->y_target = w->y - maze->MAZE_CELL_SIZE;
		}
		else
		{
			w->y_target = w->y + maze->MAZE_CELL_SIZE;
		}
	}
	else if (going_west)
	{
		if (cell->north_open)
		{
			w->x_target = w->x;
			w->y_target = w->y - maze->MAZE_CELL_SIZE;
		}
		else if (!cell->west_open)
		{
			w->x_target = w->x + maze->MAZE_CELL_SIZE;
		}
		else
		{
			w->x_target = w->x - maze->MAZE_CELL_SIZE;
		}
	}
	else if (going_east)
	{
		if (cell->south_open)
		{
			w->x_target = w->x;
			w->y_target = w->y + maze->MAZE_CELL_SIZE;
		}
		else if (!cell->east_open)
		{
			w->x_target = w->x - maze->MAZE_CELL_SIZE;
		}
		else
		{
			w->x_target = w->x + maze->MAZE_CELL_SIZE;
		}
	}
}

void updateWhirligigs()
{
	for (int index = 0; index < MAX_WHIRLIGIGS; index++)
	{
		Entity *w = &whirligigs[index];
		if (!w->is_alive)
		{
			spawnWhirligig(w);
		}

		updateWhirligigRandom(w);
		//updateWhirligigGoRight(w);
	}
}

void updateBlocks()
{
	for (int n = 0; n < MAX_BLOCKS; n++)
	{
		if (!blocks[n].is_alive)
		{
			continue;
		}
		if (SDL_GetTicks() - blocks[n].birth_time > BLOCK_MAX_AGE_MS)
		{
			blocks[n].is_alive = false;
			remaining_blocks++;
		}
	}
}

/*
 * World drawing.
 */

inline int getOffsetX(CGA::GraphicsMode *mode)
{
	int flash_delta = flash_end_time - SDL_GetTicks();
	int offset = (flash_delta > 0) ? (flash_delta % 3) - 1 : 0;
	return mode->getScreenWidth() / 2 - player.x + offset;
}

inline int getOffsetY(CGA::GraphicsMode *mode)
{
	int flash_delta = flash_end_time - SDL_GetTicks();
	int offset = (flash_delta > 0) ? (flash_delta % 3) - 1 : 0;
	return mode->getScreenHeight() / 2 - player.y + offset;
}

void drawBlocks(CGA::GraphicsMode *mode)
{
	int MAZE_OFFSET_X = getOffsetX(mode);
	int MAZE_OFFSET_Y = getOffsetY(mode);

	for (int n = 0; n < MAX_BLOCKS; n++)
	{
		if (!blocks[n].is_alive)
		{
			continue;
		}

		BlockEntity *block = &blocks[n];
		int age = SDL_GetTicks() - block->birth_time;
		int tile_index = (age / (float)BLOCK_MAX_AGE_MS) * 3;
		char ch = 219;
			 if (tile_index == 1)	ch = 178;
		else if (tile_index == 2)	ch = 177;
		else if (tile_index == 3)	ch = 176;

		mode->drawChar(MAZE_OFFSET_X + block->x + 2, MAZE_OFFSET_Y + block->y + 2, 6, 255, ch);
	}
}

const char WHIRLIGIG_TILES[] = "-\\|/";
const int SPRITE_ANIM_SPEED = 100;
int sprite_timer = 0;
int sprite_index = 0;
void drawWhirligigs(CGA::GraphicsMode *mode)
{
	int MAZE_OFFSET_X = getOffsetX(mode);
	int MAZE_OFFSET_Y = getOffsetY(mode);

	if (SDL_GetTicks() - sprite_timer >= SPRITE_ANIM_SPEED)
	{
		sprite_timer = SDL_GetTicks();
		sprite_index++;
	}

	for (int index = 0; index < MAX_WHIRLIGIGS; index++)
	{
		Entity *w = &whirligigs[index];
		if (!w->is_alive)
		{
			continue;
		}
		char ch = WHIRLIGIG_TILES[sprite_index % 4];
		mode->drawChar(MAZE_OFFSET_X + w->x + 2, MAZE_OFFSET_Y + w->y + 2, 8 + (sprite_index % 8), 255, ch);
	}
}

void drawMaze(CGA::GraphicsMode *mode)
{
	int MAZE_OFFSET_X = getOffsetX(mode);
	int MAZE_OFFSET_Y = getOffsetY(mode);
	int MAZE_COLOR = finished_maze ? 9 : 2;

	for (int row = 0; row < maze->rows; row++)
	{
		for (int column = 0; column < maze->columns; column++)
		{
			int top = MAZE_OFFSET_Y + row * maze->MAZE_CELL_SIZE;
			int bottom = top + maze->MAZE_CELL_SIZE - 1;
			int left = MAZE_OFFSET_X + column * maze->MAZE_CELL_SIZE;
			int right = left + maze->MAZE_CELL_SIZE - 1;

			MazeCell *current_cell = maze->getCell(row, column);
			if (!current_cell->north_open)
			{
				mode->drawHLine(left, right, top, MAZE_COLOR);
			}
			if (!current_cell->south_open)
			{
				mode->drawHLine(left, right, bottom, MAZE_COLOR);
			}
			if (!current_cell->east_open)
			{
				mode->drawVLine(right, top, bottom, MAZE_COLOR);
			}
			if (!current_cell->west_open)
			{
				mode->drawVLine(left, top, bottom, MAZE_COLOR);
			}

			switch (current_cell->prize_type)
			{
				case PRIZE_START:
					if (SDL_GetTicks() - clock_timer < 500)
					{
						mode->drawChar(left + 2, top + 2, 255, 10, 'S');
					}
					break;
				case PRIZE_FINISH:
					if (SDL_GetTicks() - clock_timer < 500)
					{
						mode->drawChar(left + 2, top + 2, 255, 12, 'F');
					}
					break;
				case PRIZE_FOOD:
					mode->drawChar(left + 2, top + 2, 14, 255, 7);
					break;
				case PRIZE_TIME:
					mode->drawBitmap(left + 2, top + 2, 15, 255, CLOCK);
					break;
				case PRIZE_HEALTH:
					mode->drawChar(left + 2, top + 2, 4, 255, 3);
					break;
			}

			//char buffer[5];
			//sprintf(buffer, "%d", maze->getDijkstra(row, column));
			//mode->drawString(left, top, 0x40, buffer);
		}
	}
}

void drawSolution(CGA::GraphicsMode *mode)
{
	//printf("%d, %d --> %d, %d\n", start_row, start_column, finish_row, finish_column);
	
	int MAZE_OFFSET_X = getOffsetX(mode);
	int MAZE_OFFSET_Y = getOffsetY(mode);
	int MAX_SCORE = maze->rows * maze->columns;

	int row = maze->finish_row;
	int column = maze->finish_column;

	while ((row != maze->start_row) || (column != maze->start_column))
	{
		//printf("%d, %d\n", row, column);
		
		mode->drawFilledRect(MAZE_OFFSET_X + column * maze->MAZE_CELL_SIZE, MAZE_OFFSET_X + column * maze->MAZE_CELL_SIZE + maze->MAZE_CELL_SIZE - 1,
							 MAZE_OFFSET_Y + row * maze->MAZE_CELL_SIZE, MAZE_OFFSET_Y + row * maze->MAZE_CELL_SIZE + maze->MAZE_CELL_SIZE - 1, 8);
		
		int north_score = (row - 1) >= 0 ? maze->dijkstra_map[(row - 1) * maze->columns + column] : MAX_SCORE;
		int south_score = (row + 1) < maze->rows ? maze->dijkstra_map[(row + 1) * maze->columns + column] : MAX_SCORE;
		int west_score = (column - 1) >= 0 ? maze->dijkstra_map[row * maze->columns + column - 1] : MAX_SCORE;
		int east_score = (column + 1) < maze->columns ? maze->dijkstra_map[row * maze->columns + column + 1] : MAX_SCORE;

		int target_score = maze->dijkstra_map[row * maze->columns + column] - 1;

		MazeCell *cell = maze->getCell(row, column);
		if ((north_score <= target_score) && cell->north_open)
		{
			row--;
			continue;
		}
		else if ((south_score <= target_score) && cell->south_open)
		{
			row++;
			continue;
		}
		else if ((west_score <= target_score) && cell->west_open)
		{
			column--;
			continue;
		}
		else if ((east_score <= target_score) && cell->east_open)
		{
			column++;
			continue;
		}
		else
		{
			// Something bad happened; duck out instead of looping forever.
			break;
		}
	}
}

int pause_last_blink_time = 0;
bool pause_show_text = false;
void drawPauseWindow(CGA::GraphicsMode *mode)
{
	const int BLINK_SPEED = 500;
	const int CENTER_X = mode->getScreenWidth() >> 1;
	const int CENTER_Y = mode->getScreenHeight() >> 1;

	int left = mode->getScreenWidth() * 0.3;
	int right = mode->getScreenWidth() * 0.7;
	int top = mode->getScreenHeight() * 0.3;
	int bottom = mode->getScreenHeight() * 0.7;
	mode->drawFilledRect(left, right, top, bottom, 0);

	mode->drawHLine(left, right, top, 9);
	mode->drawHLine(left, right, top + 2, 9);

	mode->drawHLine(left, right, bottom, 9);
	mode->drawHLine(left, right, bottom - 2, 9);

	mode->drawVLine(left, top, bottom, 9);
	mode->drawVLine(left + 2, top, bottom, 9);

	mode->drawVLine(right, top, bottom, 9);
	mode->drawVLine(right - 2, top, bottom, 9);

	mode->drawString(CENTER_X - 4 * 7, CENTER_Y - 4, 0xF0, "PAUSED!");

	if (SDL_GetTicks() - pause_last_blink_time >= BLINK_SPEED)
	{
		pause_last_blink_time = SDL_GetTicks();
		pause_show_text = !pause_show_text;
	}

	if (pause_show_text)
	{
		mode->drawString(CENTER_X - 4 * 11, CENTER_Y - 4 + 8 * 4, 0xF0, "PRESS <TAB>");
	}
}

void drawEverything(CGA::GraphicsMode *mode)
{
	if (SDL_GetTicks() < flash_end_time)
	{
		mode->clearGraphicsScreen(randInt(0, 8));
	}
	else
	{
		if (finished_maze)
		{
			mode->clearGraphicsScreen(2);
		}
		else if (!player.is_alive)
		{
			mode->clearGraphicsScreen(4);
		}
		else
		{
			mode->clearGraphicsScreen(1);
		}
	}

	#ifdef ALLOW_SHOW_SOLUTION
		if (show_solution)
		{
			drawSolution(mode);
		}
	#endif
	drawMaze(mode);
	player.draw(mode);
	drawWhirligigs(mode);
	drawBlocks(mode);

	drawHUD(mode);

	if (is_paused)
	{
		drawPauseWindow(mode);
	}
}

// Render the opening effect.
void openCurtainFx(CGA::GraphicsMode *mode)
{
	for (int x = mode->getScreenWidth() / 2; x >= 0; x--)
	{
		drawEverything(mode);

		mode->drawFilledRect(0, x, 8, mode->getScreenHeight(), 0);
		mode->drawFilledRect(mode->getScreenWidth() - x, mode->getScreenWidth(), 0, mode->getScreenHeight(), 0);
		
		mode->render();
	}
}

// Render the closing effect.
void closeCurtainFx(CGA::GraphicsMode *mode)
{
	for (int x = 0; x < mode->getScreenWidth() / 2; x++)
	{
		drawEverything(mode);

		mode->drawFilledRect(0, x, 8, mode->getScreenHeight(), 0);
		mode->drawFilledRect(mode->getScreenWidth() - x, mode->getScreenWidth(), 0, mode->getScreenHeight(), 0);
		
		mode->render();
	}
}

// Calculate the player's final score.  Returns true to play again.
bool finalScoreScreen(CGA::GraphicsMode *mode)
{
	int CONSUME_SCORE_SPEED = 300;
	const int BLINK_SPEED = 500;

	SDL_Event e;
	char buffer[32];
	int center_x = mode->getScreenWidth() >> 1;
	int center_y = mode->getScreenHeight() >> 1;
	int player_health = player.health;
	int player_score = player.score;
	int player_time = seconds;
	int finish_bonus = finished_maze ? 1000 : 0;
	int final_score = 0;
	bool show_text = false;
	bool score_is_consumed = false;
	int last_consume_score = 0;
	int last_blink_time = 0;

	while (true)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			KEYBOARD::respondToEvent(e);
			switch (e.type)
			{
			case SDL_QUIT:
				return false;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_y:
					return true;
				case SDLK_n:
					return false;
				}
				break;
			}
		}

		Uint8 keysym = KEYBOARD::getScanCode();
		if (keysym == SDLK_RETURN)
		{
			if (KEYBOARD::isAltPressed())
			{
				SYSTEM::toggleFullscreen();
			}
		}

		mode->clearGraphicsScreen(0);

		drawHUD(mode);

		if (SDL_GetTicks() - last_consume_score >= CONSUME_SCORE_SPEED)
		{
			last_consume_score = SDL_GetTicks();

			if (player_health > 0)
			{
				player_health--;
				//player.health--;
				final_score += HEALTH_PRIZE_SCORE;
			}
			else if (player_time > 0)
			{
				player_time--;
				//seconds--;
				final_score += TIME_VALUE;
				CONSUME_SCORE_SPEED = 80;
			}
			else if (player_score > 0)
			{
				player_score--;
				//player.score--;
				final_score += 1;
				CONSUME_SCORE_SPEED = 8;
			}
			else if (finish_bonus > 0)
			{
				finish_bonus--;
				final_score += 1;
				CONSUME_SCORE_SPEED = 4;
			}
			else
			{
				score_is_consumed = true;
			}
		}

		if (score_is_consumed)
		{
			if (SDL_GetTicks() - last_blink_time >= BLINK_SPEED)
			{
				last_blink_time = SDL_GetTicks();
				show_text = !show_text;
			}
		}

		if (show_text)
		{
			mode->drawString(center_x - 4 * 17, center_y - 4 + 8 * 4, 0xF0, "PLAY AGAIN? (Y/N)");
		}

		sprintf(buffer, "%2dx HEARTS: %d", HEALTH_PRIZE_SCORE,  player_health);
		mode->drawString(center_x - strlen(buffer) * 4, center_y - 4 - 8 * 5, 0xF0, buffer);
		sprintf(buffer, "%2dx TIME: %d", TIME_VALUE, player_time);
		mode->drawString(center_x - strlen(buffer) * 4, center_y - 4 - 8 * 4, 0xF0, buffer);
		sprintf(buffer, "%2dx SCORE: %d", 1, player_score);
		mode->drawString(center_x - strlen(buffer) * 4, center_y - 4 - 8 * 3, 0xF0, buffer);

		if (finished_maze)
		{
			sprintf(buffer, "FINISHED MAZE: %+d", finish_bonus);
			mode->drawString(center_x - strlen(buffer) * 4, center_y - 4 - 8 * 2, 0xF0, buffer);
		}

		sprintf(buffer, "FINAL SCORE: %d", final_score);
		mode->drawString(center_x - strlen(buffer) * 8 / 2, center_y - 4, 0xF0, buffer);

		mode->render();
	}
}

int main(int argc, char *argv[])
{
	CGA::GraphicsMode *mode;

	// Initialize SDL
	if (!SYSTEM::initialize(false))
	{
		printf("Failed to initialize!\n");
		SYSTEM::close();
		return 1;
	}

	// Load media
	mode = new CGA::MODE_B();
	mode->initialize();
	if (!mode->isModeSet())
	{
		printf("Failed to load media!\n");
		SYSTEM::close();
		return 1;
	}

	SDL_Event e;

	bool keep_playing = true;
	while (keep_playing)
	{
		// TODO: This initialization block should probably move to it's own function.
		bool is_done = false;
		maze = new Maze();
		setRandSeed(maze_seed);
		generateMaze(maze);

		player.jumpTo(maze->start_column * maze->MAZE_CELL_SIZE + 3, maze->start_row * maze->MAZE_CELL_SIZE + 3);
		player.is_alive = true;
		player.health = INITIAL_HEALTH;

		finished_maze = false;
		seconds = INITIAL_SECONDS;
		is_paused = true;

		for (int n = 0; n < MAX_BLOCKS; n++)
		{
			blocks[n].is_alive = false;
		}
		for (int n = 0; n < MAX_WHIRLIGIGS; n++)
		{
			whirligigs[n].is_alive = false;
		}

		openCurtainFx(mode);

		while (!is_done && player.is_alive && !finished_maze)
		{
			Uint32 frame_start_time = SDL_GetTicks();

			KEYBOARD::setScanCode(0);

			while (SDL_PollEvent(&e) != 0)
			{
				KEYBOARD::respondToEvent(e);
				switch (e.type)
				{
				case SDL_QUIT:
					is_done = true;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						is_done = true; // This is temporary.
						break;
					case SDLK_TAB:
						if (e.key.repeat == 0)
						{
							is_paused = !is_paused;
						}
						break;
	#ifdef ALLOW_SHOW_SOLUTION
					case SDLK_BACKQUOTE:
						show_solution = true;
						break;
	#endif
					}
					break;
				case SDL_KEYUP:
					switch (e.key.keysym.sym)
					{
	#ifdef ALLOW_SHOW_SOLUTION
					case SDLK_BACKQUOTE:
						show_solution = false;
						break;
	#endif
					}
					break;
				}

				//updatePlayerEvents(e);
			}

			Uint8 keysym = KEYBOARD::getScanCode();

			if (keysym == SDLK_RETURN)
			{
				if (KEYBOARD::isAltPressed())
				{
					SYSTEM::toggleFullscreen();
				}
			}

			if (!is_paused)
			{
				updatePlayerEvents();
				updatePlayer();
				updateWhirligigs();
				updateBlocks();
				updateTime();
			}

			drawEverything(mode);

			mode->render();

			Uint32 frame_elapsed = SDL_GetTicks() - frame_start_time;
			if (frame_elapsed < (Uint32)SCREEN_TICKS_PER_FRAME)
			{
				SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_elapsed);
			}
		}

		closeCurtainFx(mode);

		delete maze;

		// TODO: There should be variable lengths of path from start to finish.

		// Calculate the final score.
		keep_playing = finalScoreScreen(mode);
		if (finished_maze)
		{
			maze_seed++;
		}
	}

	if (mode != NULL)
	{
		delete mode;
		mode = NULL;
	}

	SYSTEM::close();

	return 0;
}

/*

This block of code will give you an ASCII table.

CGA::TextMode* mode;

int main(int argc, char* argv[])
{
	// Initialize SDL
	if (!SYSTEM::initialize(false))
	{
		printf("Failed to initialize!\n");
		SYSTEM::close();
		return 1;
	}

	// Load media
	mode = new CGA::MODE_A();
	mode->initialize();
	if (!mode->isModeSet())
	{
		printf("Failed to load media!\n");
		SYSTEM::close();
		return 1;
	}

	mode->writeString(0, 2, 0x70, "0123456789ABCDEF");
	mode->writeChar(2, 0, 0x70, '0');
	mode->writeChar(3, 0, 0x70, '1');
	mode->writeChar(4, 0, 0x70, '2');
	mode->writeChar(5, 0, 0x70, '3');
	mode->writeChar(6, 0, 0x70, '4');
	mode->writeChar(7, 0, 0x70, '5');
	mode->writeChar(8, 0, 0x70, '6');
	mode->writeChar(9, 0, 0x70, '7');
	mode->writeChar(10, 0, 0x70, '8');
	mode->writeChar(11, 0, 0x70, '9');
	mode->writeChar(12, 0, 0x70, 'A');
	mode->writeChar(13, 0, 0x70, 'B');
	mode->writeChar(14, 0, 0x70, 'C');
	mode->writeChar(15, 0, 0x70, 'D');
	mode->writeChar(16, 0, 0x70, 'E');
	mode->writeChar(17, 0, 0x70, 'F');

	mode->writeChar(1, 1, 0xF0, 0xC9);
	mode->writeChar(1, 18, 0xF0, 0xBB);
	mode->writeChar(18, 1, 0xF0, 0xC8);
	mode->writeChar(18, 18, 0xF0, 0xBC);
	for (Uint8 n = 2; n <= 17; n++)
	{
		mode->writeChar(1, n, 0xF0, 0xCD);
		mode->writeChar(18, n, 0xF0, 0xCD);

		mode->writeChar(n, 1, 0xF0, 0xBA);
		mode->writeChar(n, 18, 0xF0, 0xBA);
	}

	mode->writeString(19, 2, 0xF0, "HEX:");
	mode->writeString(20, 2, 0xF0, "DEC:");

	// Write out the entire ASCII Table.
	Uint16 row = 0;
	Uint16 column = 0;
	for (int ch = 0; ch < 256; ch++)
	{
		mode->writeChar(row + 2, column + 2, 0x70, (unsigned char)ch);
		column++;
		if (column >= 16)
		{
			row++;
			column = 0;
		}
	}

	char selectedRow = 5;
	char selectedColumn = 8;
	mode->writeInteger(19, 6, 0x70, selectedRow * 16 + selectedColumn, 16, 2);
	mode->writeInteger(20, 6, 0x70, selectedRow * 16 + selectedColumn);
	mode->setAttr(2 + selectedRow, 0, 0x0F);
	mode->setAttr(0, 2 + selectedColumn, 0x0F);

	bool isDone = false;
	SDL_Event e;
	Uint32 lastBlinkTime = 0;
	bool isBlinked = false;

	while (!isDone)
	{
		Uint32 currentTime = SDL_GetTicks();

		KEYBOARD::setScanCode(0);

		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isDone = true;
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym)
				{
				case SDLK_LALT:
				case SDLK_RALT:
					KEYBOARD::setAltPressed(false);
					break;
				case SDLK_LCTRL:
				case SDLK_RCTRL:
					KEYBOARD::setCtrlPressed(false);
					break;
				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					KEYBOARD::setShiftPressed(false);
					break;
				}
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_LALT:
				case SDLK_RALT:
					KEYBOARD::setAltPressed(true);
					break;
				case SDLK_LCTRL:
				case SDLK_RCTRL:
					KEYBOARD::setCtrlPressed(true);
					break;
				case SDLK_LSHIFT:
				case SDLK_RSHIFT:
					KEYBOARD::setShiftPressed(true);
					break;

				case SDLK_ESCAPE:
					// TODO: Dump memory to file.  "Are you sure you want to quit?"
					// TODO: Maybe change this to Ctrl+Alt+Delete?
					isDone = true; // This is temporary.

				default:
					KEYBOARD::setScanCode(e.key.keysym.scancode);
					//printf("SCANCODE: %d\n", KEYBOARD::getScanCode());
					break;
				}

				break;
			}
		}

		Uint8 keysym = KEYBOARD::getScanCode();

		Uint8 lastSelectedRow = selectedRow;
		Uint8 lastSelectedColumn = selectedColumn;

		if (keysym == SDLK_RETURN)
		{
			if (KEYBOARD::isAltPressed())
			{
				SYSTEM::toggleFullscreen();
			}
		}
		else if ((keysym == SDL_SCANCODE_UP) || (keysym == SDL_SCANCODE_KP_8))
		{
			selectedRow--;
		}
		else if ((keysym == SDL_SCANCODE_DOWN) || (keysym == SDL_SCANCODE_KP_2))
		{
			selectedRow++;
		}
		else if ((keysym == SDL_SCANCODE_LEFT) || (keysym == SDL_SCANCODE_KP_4))
		{
			selectedColumn--;
		}
		else if ((keysym == SDL_SCANCODE_RIGHT) || (keysym == SDL_SCANCODE_KP_6))
		{
			selectedColumn++;
		}
		else if (keysym == SDL_SCANCODE_KP_7)
		{
			selectedRow--;
			selectedColumn--;
		}
		else if (keysym == SDL_SCANCODE_KP_9)
		{
			selectedRow--;
			selectedColumn++;
		}
		else if (keysym == SDL_SCANCODE_KP_1)
		{
			selectedRow++;
			selectedColumn--;
		}
		else if (keysym == SDL_SCANCODE_3)
		{
			selectedRow++;
			selectedColumn++;
		}

		if ((lastSelectedRow != selectedRow) || (lastSelectedColumn != selectedColumn))
		{
			if (selectedRow < 0)
			{
				selectedRow = 15;
			}
			else if (selectedRow > 15)
			{
				selectedRow = 0;
			}
			if (selectedColumn < 0)
			{
				selectedColumn = 15;
			}
			else if (selectedColumn > 15)
			{
				selectedColumn = 0;
			}

			mode->setAttr(2 + lastSelectedRow, 0, 0x70);
			mode->setAttr(0, 2 + lastSelectedColumn, 0x70);

			mode->setAttr(2 + selectedRow, 0, 0x0F);
			mode->setAttr(0, 2 + selectedColumn, 0x0F);

			mode->setAttr(2 + lastSelectedRow, 2 + lastSelectedColumn, 0x70);
			mode->setAttr(2 + selectedRow, 2 + selectedColumn, 0x0F);
			mode->writeInteger(19, 6, 0x70, selectedRow * 16 + selectedColumn, 16, 2);
			mode->writeString(20, 6, 0x70, "   ");
			mode->writeInteger(20, 6, 0x70, selectedRow * 16 + selectedColumn);
			lastBlinkTime = currentTime;
		}

		if (currentTime - lastBlinkTime >= 500)
		{
			isBlinked = !isBlinked;
			mode->setAttr(2 + selectedRow, 2 + selectedColumn, isBlinked ? 0x70 : 0x0F);
			lastBlinkTime = currentTime;
		}

		mode->render();
	}

	if (mode != NULL)
	{
		delete mode;
		mode = NULL;
	}

	SYSTEM::close();

	return 0;
}
*/
