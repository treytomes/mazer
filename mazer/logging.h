#pragma once

#include <SDL.h>
#include <stdio.h>

#define ALLOW_LOGGING

#ifdef ALLOW_LOGGING
	#define LOG(text) printf("%s: %s\n", __FUNCTION__, text)
	#define LOG_ENTER() int __ENTERTIME = SDL_GetTicks(); LOG("Enter.")
	#define LOG_LEAVE() printf("%s: %s (%dms)\n", __FUNCTION__, "Leave.", SDL_GetTicks() - __ENTERTIME)
#else
	#define LOG(TEXT)
	#define LOG_ENTER()
	#define LOG_LEAVE()
#endif
