// Currently unused.  SDL_GetTicks and SDL_Delay are giving me everythin I need.

#pragma once

#include <SDL.h>

class Timer
{
public:
	Timer();
	
	// The various clock actions.
	void start();
	void stop();
	void pause();
	void unpause();
	
	// Gets the timer's time.
	Uint32 getTicks();
	
	// Checks the status of the timer.
	bool isStarted();
	bool isPaused();

private:
	// The clock time when the timer started.
	Uint32 _start_ticks;
	
	// The ticks stored when the timer was paused.
	Uint32 _paused_ticks;
	
	//The timer status.
	bool _paused;
	bool _started;
};