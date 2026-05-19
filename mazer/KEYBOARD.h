#pragma once

#include "SYSTEM/SYSTEM.h"

// Set the bit on a bitflag.
#define SET_BIT(value, bit_num) value |= 1 << bit_num

// Clear the bit on a bitflag.
#define CLEAR_BIT(value, bit_num) value &= ~(1 << bit_num)

// Toggle the bit on a bitflag.
#define TOGGLE_BIT(value, bit_num) value ^= 1 << bit_num

// Check the bit on a bitflag
#define CHECK_BIT(value, bit_num) (value >> bit_num) & 1

namespace KEYBOARD
{
	const int MEM_KEYFLAGS = SYSTEM::MEM_BDA + 0x00;
	const int MEM_SCANCODE = SYSTEM::MEM_BDA + 0x01;

	const int ALT_BIT = 0;
	const int CTRL_BIT = 1;
	const int SHIFT_BIT = 2;

	void setAltPressed(bool value)
	{
		if (value)
		{
			SET_BIT(SYSTEM::memory[MEM_KEYFLAGS], ALT_BIT);
		}
		else
		{
			CLEAR_BIT(SYSTEM::memory[MEM_KEYFLAGS], ALT_BIT);
		}
	}

	void setCtrlPressed(bool value)
	{
		if (value)
		{
			SET_BIT(SYSTEM::memory[MEM_KEYFLAGS], CTRL_BIT);
		}
		else
		{
			CLEAR_BIT(SYSTEM::memory[MEM_KEYFLAGS], CTRL_BIT);
		}
	}

	void setShiftPressed(bool value)
	{
		if (value)
		{
			SET_BIT(SYSTEM::memory[MEM_KEYFLAGS], SHIFT_BIT);
		}
		else
		{
			CLEAR_BIT(SYSTEM::memory[MEM_KEYFLAGS], SHIFT_BIT);
		}
	}

	bool isAltPressed()
	{
		return CHECK_BIT(SYSTEM::memory[MEM_KEYFLAGS], ALT_BIT);
	}

	bool isCtrlPressed()
	{
		return CHECK_BIT(SYSTEM::memory[MEM_KEYFLAGS], CTRL_BIT);
	}

	bool isShiftPressed()
	{
		return CHECK_BIT(SYSTEM::memory[MEM_KEYFLAGS], SHIFT_BIT);
	}

	unsigned char getScanCode()
	{
		return SYSTEM::memory[MEM_SCANCODE];
	}

	void setScanCode(unsigned char value)
	{
		SYSTEM::memory[MEM_SCANCODE] = value;
	}

	void respondToEvent(SDL_Event &e)
	{
		switch (e.type)
		{
		case SDL_KEYUP:
			switch (e.key.keysym.sym)
			{
			case SDLK_LALT:
			case SDLK_RALT:
				setAltPressed(false);
				break;
				
			case SDLK_LCTRL:
			case SDLK_RCTRL:
				setCtrlPressed(false);
				break;

			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				setShiftPressed(false);
				break;
			}
			break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_LALT:
			case SDLK_RALT:
				setAltPressed(true);
				break;

			case SDLK_LCTRL:
			case SDLK_RCTRL:
				setCtrlPressed(true);
				break;

			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				setShiftPressed(true);
				break;

			default:
				KEYBOARD::setScanCode(e.key.keysym.scancode);
				//printf("SCANCODE: %d\n", KEYBOARD::getScanCode());
				break;
			}

			break;
		}
	}
}
