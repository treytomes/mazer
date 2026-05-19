#include "Timer.h"

Timer::Timer()
{
	// Initialize the variables.
	_start_ticks = 0;
	_paused_ticks = 0;
	_paused = false;
	_started = false;
}

void Timer::start()
{
	// Start the timer.
	_started = true;

	// Unpause the timer.
	_paused = false;
	
	// Get the current clock time.
	_start_ticks = SDL_GetTicks();
	_paused_ticks = 0;
}

void Timer::stop()
{
	// Stop the timer.
	_started = false;

	// Unpause the timer.
	_paused = false;
	
	// Clear tick variables.
	_start_ticks = 0;
	_paused_ticks = 0;
}

void Timer::pause()
{
	// If the timer is running and isn't already paused.
	if (_started && !_paused)
	{
		// Pause the timer.
		_paused = true;
		
		// Calculate the paused ticks.
		_paused_ticks = SDL_GetTicks() - _start_ticks;
		_start_ticks = 0;
	}
}

void Timer::unpause()
{
	// If the timer is running and paused.
	if (_started && _paused)
	{
		// Unpause the timer.
		_paused = false;
		
		// Reset the starting ticks.
		_start_ticks = SDL_GetTicks() - _paused_ticks;
		
		// Reset the paused ticks.
		_paused_ticks = 0;
	}
}

Uint32 Timer::getTicks()
{
	// The actual timer time.
	Uint32 time = 0;
	
	// If the timer is running.
	if (_started)
	{
		// If the timer is paused.
		if (_paused)
		{
			// Return the number of ticks when the timer was paused.
			time = _paused_ticks;
		}
		else
		{
			// Return the current time minus the start time.
			time = SDL_GetTicks() - _start_ticks;
		}
	}
	return time;
}

bool Timer::isStarted()
{
	// Timer is running and paused or unpaused.
	return _started;
}

bool Timer::isPaused()
{
	// Timer is running and paused.
	return _paused && _started;
}