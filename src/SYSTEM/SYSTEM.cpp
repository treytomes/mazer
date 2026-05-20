#include <stdio.h>

#include "../OEM437.h"
#include "SYSTEM.h"

// The window we'll be rendering to
SDL_Window* SYSTEM::window = NULL;

// The surface contained by the window
SDL_Surface* SYSTEM::screenSurface = NULL;

//// Graphics operations will write to this array.
//uint8_t* SYSTEM::graphicsMemory = NULL;

// Everything the program uses will be stored here.
uint8_t* SYSTEM::memory = NULL;
//uint8_t* SYSTEM::textMemory = NULL;

/*
 * Compute the largest integer scale that fits in the usable desktop area
 * (excludes taskbars) with a margin for window chrome (title bar, borders).
 */
int getScreenScale()
{
	SDL_Rect usable;
	if (SDL_GetDisplayUsableBounds(0, &usable) != 0)
	{
		// Fallback: use display mode minus a conservative chrome reserve.
		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		usable = { 0, 0, dm.w, dm.h };
	}
	// Reserve ~60px for title bar + borders so the window fits visibly.
	const int CHROME_RESERVE = 60;
	int scaleX = usable.w / SYSTEM::SCREEN_WIDTH;
	int scaleY = (usable.h - CHROME_RESERVE) / SYSTEM::SCREEN_HEIGHT;
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	if (scale < 1) scale = 1;
	return scale;
}

bool SYSTEM::initialize(bool fullscreen, const char* title)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	int scale = getScreenScale();
	int windowWidth = SCREEN_WIDTH * scale;
	int windowHeight = SCREEN_HEIGHT * scale;

	// Create window
	int windowFlags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, windowFlags);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// Get window surface
	screenSurface = SDL_GetWindowSurface(window);

	memory = (uint8_t*)calloc(MEM_SIZE, 1);
	if (memory == NULL)
	{
		printf("Unable to allocate memory.\n");
		return false;
	}

	//SYSTEM::surfacePixels = (uint8_t*)calloc(CGA::MODE0::SCREEN_WIDTH * CGA::MODE0::SCREEN_HEIGHT, 1);
	//graphicsMemory = memory + MEM_GRAPHICS;

	//SYSTEM::textMemory = (Uint8*)calloc(CGA::MODE0::TEXT_ROWS * CGA::MODE0::TEXT_COLUMNS * BYTES_PER_CHAR * CGA::MODE0::PAGES, 1);
	//textMemory = memory + MEM_TEXT;

	// Load the ASCII table into memory.
	for (int index = 0; index < 256 * 8; index++) // 256 characters, 8 bytes per character.
	{
		memory[MEM_ASCII + index] = OEM437::DATA[index];
	}

	return true;
}

void SYSTEM::close()
{
	if (memory != NULL)
	{
		free(memory);
		//delete[] gSurfacePixels;
		memory = NULL;
		//graphicsMemory = NULL;
		//textMemory = NULL;
	}

	if (window != NULL)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	//Quit SDL subsystems
	SDL_Quit();
}

void SYSTEM::toggleFullscreen()
{
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	bool isFullscreen = SDL_GetWindowFlags(SYSTEM::window) & fullscreenFlag;

	if (isFullscreen)
	{
		// Re-add borders before leaving fullscreen so the WM redecorates the window.
		SDL_SetWindowBordered(SYSTEM::window, SDL_TRUE);
	}

	int result = SDL_SetWindowFullscreen(SYSTEM::window, isFullscreen ? 0 : fullscreenFlag);
	if (result < 0)
	{
		printf("Unable to toggle fullscreen.  SDL error: %s\n", SDL_GetError());
		return;
	}

	if (isFullscreen)
	{
		// Restore to the computed windowed size and re-center.
		int scale = getScreenScale();
		SDL_SetWindowSize(SYSTEM::window, SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale);
		SDL_SetWindowPosition(SYSTEM::window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	SYSTEM::screenSurface = SDL_GetWindowSurface(SYSTEM::window);
}
