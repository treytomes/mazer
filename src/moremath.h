#pragma once

/*
inline void setRandSeed(int seed);

inline int randInt(int inclusiveMin, int exclusiveMax);

inline bool isInRange(float value, float inclusive_min, float exclusive_max);

inline int min(int a, int b);
*/

#include <math.h>
#include <stdlib.h>

inline void setRandSeed(int seed)
{
	srand(seed);
}

inline int randInt(int inclusiveMin, int exclusiveMax)
{
	if (exclusiveMax - inclusiveMin == 0)
	{
		return inclusiveMin;
	}
	return inclusiveMin + (rand() % (exclusiveMax - inclusiveMin));
}

inline bool randBool()
{
	return randInt(0, 2) == 1;
}

inline bool isInRange(float value, float inclusive_min, float exclusive_max)
{
	return (inclusive_min <= value) && (value < exclusive_max);
}

inline int min(int a, int b)
{
	return (a < b) ? a : b;
}

inline int max(int a, int b)
{
	return (a > b) ? a : b;
}

inline float distance(int x1, int y1, int x2, int y2)
{
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
