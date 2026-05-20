#pragma once

#include <SDL.h>

namespace SYSTEM
{
	const int SCREEN_WIDTH = 320;
	const int SCREEN_HEIGHT = 240;
	
	const int MEM_SIZE = 0xFFFFFF;

	// Interrupt table runs from 0x00000 to 0x003FF.
	const int MEM_INTERRUPT_TABLE = 0x00000;

	// BIOS data area runs from 0x00400 to 0x4FF.
	const int MEM_BDA = 0x00400;

	//// Video RAM runs from 0xA0000 to 0xBFFFF.
	//const int MEM_GRAPHICS = 0xA0000;
	//const int MEM_TEXT = 0xB8000;

	// Motherboard BIOS runs from 0xF0000 to 0xFFFFF.  Store the OS here?
	// I don't know if I want to load the table into memory, or reference it separately.  I'll decide later.
	const int MEM_ASCII = 0xFFA6E;

	// Starts up SDL and creates window
	bool initialize(bool fullscreen, const char* title = "mazer");

	// Frees media and shuts down SDL
	void close();

	void toggleFullscreen();

	// The window we'll be rendering to
	extern SDL_Window* window;

	// The surface contained by the window
	extern SDL_Surface* screenSurface;

	// Returns true if the window surface was re-acquired (caller should reinitializeSurface).
	// Call inside every SDL_PollEvent loop to handle fullscreen transitions.
	inline bool handleWindowEvent(const SDL_Event& e)
	{
		if (e.type == SDL_WINDOWEVENT &&
		    e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		{
			screenSurface = SDL_GetWindowSurface(window);
			return true;
		}
		return false;
	}

	// Everything the program uses will be stored here.
	extern uint8_t* memory;

	// Can't really simulate CGA graphics memory, as I can't seem to go lower than 8-bits per pixel.  Text and graphics memory will overlap in that case.
	//// Graphics operations will write to this array.
	//extern uint8_t* graphicsMemory;

	inline SDL_Color convertHtmlColor(unsigned int htmlColor)
	{
		SDL_Color color;
		color.r = htmlColor >> 16;
		color.g = (htmlColor >> 8) & 0xFF;
		color.b = htmlColor & 0xFF;
		color.a = 0;
		return color;
	}

	// Uses the "Luma" formula: Gray = (Red * 0.2126 + Green * 0.7152 + Blue * 0.0722)
	inline SDL_Color convertHtmlGrayscale(unsigned int htmlColor)
	{
		SDL_Color color;
		Uint8 gray = (Uint8)((htmlColor >> 16) * 0.2126 + ((htmlColor >> 8) & 0xFF) * 0.7152 + (htmlColor & 0xFF) * 0.0722);
		color.r = color.g = color.b = gray;
		color.a = 0;
		return color;
	}
}
