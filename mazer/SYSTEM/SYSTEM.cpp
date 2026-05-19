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
 * Make the window as big as possible for the current screen resolution.
 */
int getScreenScale()
{
	SDL_DisplayMode dm;
	SDL_GetCurrentDisplayMode(0, &dm);
	int scaleX = (int)(dm.w / SYSTEM::SCREEN_WIDTH);
	int scaleY = (int)(dm.h / SYSTEM::SCREEN_HEIGHT);
	int scale = (scaleX < scaleY) ? scaleX : scaleY;
	return scale;
}

bool SYSTEM::initialize(bool fullscreen)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	int scale = getScreenScale();
	int windowWidth = SCREEN_WIDTH * scale;
	int windowHeight = SCREEN_HEIGHT * scale;

	// Create window
	int windowFlags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
	window = SDL_CreateWindow("DOSSIM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, windowFlags);
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
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN;
	bool isFullscreen = SDL_GetWindowFlags(SYSTEM::window) & fullscreenFlag;

	//if (isFullscreen)
	//{
	//	SDL_SetWindowSize(SYSTEM::window, SYSTEM::WINDOW_WIDTH, SYSTEM::WINDOW_HEIGHT);
	//}
	//else
	//{
	//	SDL_SetWindowSize(SYSTEM::window, mode->getScreenWidth(), mode->getScreenHeight());
	//}

	int result = SDL_SetWindowFullscreen(SYSTEM::window, isFullscreen ? 0 : fullscreenFlag);
	if (result < 0)
	{
		printf("Unable to toggle fullscreen.  SDL error: %s\n", SDL_GetError());
	}

	SYSTEM::screenSurface = SDL_GetWindowSurface(SYSTEM::window);
}
