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
	const int MEM_KEYFLAGS  = SYSTEM::MEM_BDA + 0x00;
	const int MEM_SCANCODE  = SYSTEM::MEM_BDA + 0x01;
	// Gamepad directional state bits (held this frame).
	// Bit layout: 0=up, 1=down, 2=left, 3=right, 4=A(confirm), 5=B(back), 6=shoulder
	const int MEM_GAMEPAD   = SYSTEM::MEM_BDA + 0x02;

	const int GP_UP_BIT        = 0;
	const int GP_DOWN_BIT      = 1;
	const int GP_LEFT_BIT      = 2;
	const int GP_RIGHT_BIT     = 3;
	const int GP_CONFIRM_BIT   = 4;
	const int GP_BACK_BIT      = 5;
	const int GP_SHOULDER_BIT  = 6; // right shoulder = place block / show solution

	// Axis threshold: ~50% of Sint16 range.
	const int AXIS_THRESHOLD = 16384;

	const int ALT_BIT = 0;
	const int CTRL_BIT = 1;
	const int SHIFT_BIT = 2;

	inline void setAltPressed(bool value)
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

	inline void setCtrlPressed(bool value)
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

	inline void setShiftPressed(bool value)
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

	inline bool isAltPressed()
	{
		return CHECK_BIT(SYSTEM::memory[MEM_KEYFLAGS], ALT_BIT);
	}

	inline bool isCtrlPressed()
	{
		return CHECK_BIT(SYSTEM::memory[MEM_KEYFLAGS], CTRL_BIT);
	}

	inline bool isShiftPressed()
	{
		return CHECK_BIT(SYSTEM::memory[MEM_KEYFLAGS], SHIFT_BIT);
	}

	inline unsigned char getScanCode()
	{
		return SYSTEM::memory[MEM_SCANCODE];
	}

	inline void setScanCode(unsigned char value)
	{
		SYSTEM::memory[MEM_SCANCODE] = value;
	}

	inline bool isGamepadHeld(int bit)
	{
		return CHECK_BIT(SYSTEM::memory[MEM_GAMEPAD], bit);
	}

	inline bool isGamepadUp()      { return isGamepadHeld(GP_UP_BIT);       }
	inline bool isGamepadDown()    { return isGamepadHeld(GP_DOWN_BIT);     }
	inline bool isGamepadLeft()    { return isGamepadHeld(GP_LEFT_BIT);     }
	inline bool isGamepadRight()   { return isGamepadHeld(GP_RIGHT_BIT);    }
	inline bool isGamepadConfirm() { return isGamepadHeld(GP_CONFIRM_BIT);  }
	inline bool isGamepadBack()    { return isGamepadHeld(GP_BACK_BIT);     }
	inline bool isGamepadShoulder(){ return isGamepadHeld(GP_SHOULDER_BIT); }

	inline void respondToEvent(SDL_Event &e)
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
				break;
			}
			break;

		// ---- Gamepad hotplug ----
		case SDL_CONTROLLERDEVICEADDED:
			if (SYSTEM::controller == NULL)
				SYSTEM::controller = SDL_GameControllerOpen(e.cdevice.which);
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
			if (SYSTEM::controller != NULL &&
			    SDL_GameControllerGetJoystick(SYSTEM::controller) ==
			    SDL_JoystickFromInstanceID(e.cdevice.which))
			{
				SDL_GameControllerClose(SYSTEM::controller);
				SYSTEM::controller = NULL;
				// Try to open another controller if one is still connected.
				for (int i = 0; i < SDL_NumJoysticks(); i++)
				{
					if (SDL_IsGameController(i))
					{
						SYSTEM::controller = SDL_GameControllerOpen(i);
						break;
					}
				}
			}
			break;

		// ---- Gamepad buttons: inject scancode for menu navigation ----
		case SDL_CONTROLLERBUTTONDOWN:
		{
			SDL_GameControllerButton btn =
				(SDL_GameControllerButton)e.cbutton.button;
			switch (btn)
			{
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_UP_BIT);
				setScanCode(SDL_SCANCODE_UP);
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_DOWN_BIT);
				setScanCode(SDL_SCANCODE_DOWN);
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_LEFT_BIT);
				setScanCode(SDL_SCANCODE_LEFT);
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_RIGHT_BIT);
				setScanCode(SDL_SCANCODE_RIGHT);
				break;
			case SDL_CONTROLLER_BUTTON_A:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_CONFIRM_BIT);
				setScanCode(SDL_SCANCODE_RETURN);
				break;
			case SDL_CONTROLLER_BUTTON_B:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_BACK_BIT);
				setScanCode(SDL_SCANCODE_ESCAPE);
				break;
			case SDL_CONTROLLER_BUTTON_START:
				setScanCode(SDL_SCANCODE_RETURN); // pause / confirm
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_SHOULDER_BIT);
				break;
			default:
				break;
			}
			break;
		}

		case SDL_CONTROLLERBUTTONUP:
		{
			SDL_GameControllerButton btn =
				(SDL_GameControllerButton)e.cbutton.button;
			switch (btn)
			{
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_UP_BIT);    break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_DOWN_BIT);  break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_LEFT_BIT);  break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_RIGHT_BIT); break;
			case SDL_CONTROLLER_BUTTON_A:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_CONFIRM_BIT); break;
			case SDL_CONTROLLER_BUTTON_B:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_BACK_BIT);  break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_SHOULDER_BIT); break;
			default:
				break;
			}
			break;
		}

		// ---- Left stick / left trigger: held state for gameplay ----
		case SDL_CONTROLLERAXISMOTION:
		{
			SDL_GameControllerAxis axis =
				(SDL_GameControllerAxis)e.caxis.axis;
			Sint16 val = e.caxis.value;
			if (axis == SDL_CONTROLLER_AXIS_LEFTX)
			{
				if (val < -AXIS_THRESHOLD)
				{
					SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_LEFT_BIT);
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_RIGHT_BIT);
				}
				else if (val > AXIS_THRESHOLD)
				{
					SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_RIGHT_BIT);
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_LEFT_BIT);
				}
				else
				{
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_LEFT_BIT);
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_RIGHT_BIT);
				}
			}
			else if (axis == SDL_CONTROLLER_AXIS_LEFTY)
			{
				if (val < -AXIS_THRESHOLD)
				{
					SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_UP_BIT);
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_DOWN_BIT);
				}
				else if (val > AXIS_THRESHOLD)
				{
					SET_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_DOWN_BIT);
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_UP_BIT);
				}
				else
				{
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_UP_BIT);
					CLEAR_BIT(SYSTEM::memory[MEM_GAMEPAD], GP_DOWN_BIT);
				}
			}
			break;
		}
		}
	}
}
