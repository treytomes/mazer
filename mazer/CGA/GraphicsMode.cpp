#include "GraphicsMode.h"
#include <stdio.h>
#include "../OEM437.h"

CGA::GraphicsMode::GraphicsMode()
{
	_convertedSurface = NULL;
	_drawingSurface = NULL;
}

CGA::GraphicsMode::~GraphicsMode()
{
	if (_convertedSurface != NULL)
	{
		SDL_FreeSurface(_convertedSurface);
		_convertedSurface = NULL;
	}
	if (_drawingSurface != NULL)
	{
		SDL_FreeSurface(_drawingSurface);
		_drawingSurface = NULL;
	}
	if (_graphicsMemory != NULL)
	{
		free(_graphicsMemory);
		_graphicsMemory = NULL;
	}
}

void CGA::GraphicsMode::initialize()
{
	SDL_PixelFormat* format = SYSTEM::screenSurface->format;
	_convertedSurface = SDL_CreateRGBSurfaceWithFormat(0, getScreenWidth(), getScreenHeight(), format->BitsPerPixel, format->format);

	_graphicsMemory = (uint8_t*)calloc(getScreenWidth() * getScreenHeight(), 1);
	_drawingSurface = SDL_CreateRGBSurfaceWithFormatFrom(_graphicsMemory, getScreenWidth(), getScreenHeight(), getBitsPerPixel(), getScreenWidth(), SDL_PIXELFORMAT_INDEX8);
}

bool CGA::GraphicsMode::isModeSet()
{
	return _isModeSet;
}

bool CGA::GraphicsMode::setPalette(SDL_Color* colors)
{
	if (SDL_SetPaletteColors(_drawingSurface->format->palette, colors, 0, 1 << getBitsPerPixel()) < 0)
	{
		printf("Unable to set the palette. SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

bool CGA::GraphicsMode::render()
{
	if (SDL_BlitSurface(_drawingSurface, NULL, _convertedSurface, NULL) < 0)
	{
		printf("Unable to convert screen format.  SDL error: %s\n", SDL_GetError());
		return false;
	}
	if (SDL_BlitScaled(_convertedSurface, NULL, SYSTEM::screenSurface, NULL) < 0)
	{
		printf("Unable to blit converted screen to window.  SDL error: %s\n", SDL_GetError());
		return false;
	}

	// Update the surface
	SDL_UpdateWindowSurface(SYSTEM::window);
	return true;
}

void CGA::GraphicsMode::drawLine(int x1, int y1, int x2, int y2, Uint8 color)
{
	int dx = x2 - x1;
	if (dx < 0)
	{
		dx = -dx;
	}
	int sx = (x1 < x2) ? 1 : -1;
	int dy = y2 - y1;
	if (dy < 0)
	{
		dy = -dy;
	}
	dy = -dy;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx + dy;
	while (true)
	{
		setPixel(x1, y1, color);
		if ((x1 == x2) && (y1 == y2))
		{
			break;
		}
		int e2 = err << 1;
		if (e2 >= dy)
		{
			err += dy;
			x1 += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y1 += sy;
		}
	}
}

void CGA::GraphicsMode::drawCircle(int xc, int yc, int r, Uint8 color)
{
	int x = 0;
	int y = r;
	int d = 3 - (r << 1);
	while (y >= x)
	{
		drawCirclePoints(xc, yc, x, y, color);
		// For each pixel we will draw all 8 points.
		x++;

		// Check for decision parameter and correspondingly update d, x, y.
		if (d > 0)
		{
			y--;
			d += ((x - y) << 2) + 10;
		}
		else
		{
			d += (x << 2) + 6;
		}
	}
}

void CGA::GraphicsMode::drawHLine(int x1, int x2, int y, Uint8 color)
{
	if ((y < 0) || (y >= getScreenHeight()))
	{
		return;
	}
	if (((x1 < 0) && (x2 < 0)) || ((x1 >= getScreenWidth()) && (x2 >= getScreenWidth())))
	{
		return;
	}

	if (x1 > x2)
	{
		int temp = x2;
		x2 = x1;
		x1 = temp;
	}
	if (x1 < 0)
	{
		x1 = 0;
	}
	else if (x1 >= getScreenWidth())
	{
		x1 = getScreenWidth() - 1;
	}
	if (x2 < 0)
	{
		x2 = 0;
	}
	else if (x2 >= getScreenWidth())
	{
		x2 = getScreenWidth() - 1;
	}
	for (int x = x1; x <= x2; x++)
	{
		setPixel(x, y, color);
	}
}

void CGA::GraphicsMode::drawVLine(int x, int y1, int y2, Uint8 color)
{
	if ((x < 0) || (x >= getScreenWidth()))
	{
		return;
	}
	if (((y1 < 0) && (y2 < 0)) || ((y1 >= getScreenHeight()) && (y2 >= getScreenHeight())))
	{
		return;
	}

	if (y1 > y2)
	{
		int temp = y2;
		y2 = y1;
		y1 = temp;
	}
	if (y1 < 0)
	{
		y1 = 0;
	}
	else if (y1 >= getScreenHeight())
	{
		y1 = getScreenHeight() - 1;
	}
	if (y2 < 0)
	{
		y2 = 0;
	}
	else if (y2 >= getScreenHeight())
	{
		y2 = getScreenHeight() - 1;
	}
	for (int y = y1; y <= y2; y++)
	{
		setPixel(x, y, color);
	}
}

void CGA::GraphicsMode::drawFilledRect(int x1, int x2, int y1, int y2, Uint8 color)
{
	/*
	if (((x1 < 0) && (x2 < 0)) || ((x1 >= getScreenWidth()) && (x2 >= getScreenWidth()) ||
		((y1 < 0) && (y2 < 0)) || ((y1 >= getScreenHeight()) && (y2 >= getScreenHeight()))))
	{
		return;
	}
	if (x1 < 0)
	{
		x1 = 0;
	}
	if (x2 >= getScreenWidth())
	{
		x2 = getScreenWidth() - 1;
	}
	if (y1 < 0)
	{
		y1 = 0;
	}
	if (y2 >= getScreenHeight())
	{
		y2 = getScreenHeight() - 1;
	}
	
	Uint32 index = y1 * getScreenWidth() + x1;
	Uint16 width = x2 - x1 + 1;
	for (Uint16 y = y1; y <= y2; y++)
	{
		for (Uint16 x = 0; x < width; x++)
		{
			_graphicsMemory[index] = color;
			index++;
		}
		index += getScreenWidth() - width;
	}
	*/

	if (x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	for (int y = y1; y <= y2; y++)
	{
		for (int x = x1; x <= x2; x++)
		{
			setPixel(x, y, color);
		}
	}
}

void CGA::GraphicsMode::drawBitmap(int x, int y, Uint8 fgColor, Uint8 bgColor, unsigned char *bitmap)
{
	for (Uint8 yc = 0; yc < OEM437::CHAR_HEIGHT; yc++, bitmap++)
	{
		Uint8 charByte = *bitmap;
		for (Uint8 xc = 0; xc < OEM437::CHAR_WIDTH; xc++, charByte = charByte >> 1)
		{
			if ((charByte & 1) != 0)
			{
				setPixel(x + xc, y + yc, fgColor);
			}
			else if (bgColor != 255)
			{
				setPixel(x + xc, y + yc, bgColor);
			}
		}
	}
}

/*
void CGA::GraphicsMode::drawCharInternal(Uint32 screenIndex, Uint8 fgColor, Uint8 bgColor, unsigned char ch)
{
	Uint32 charIndex = ch * OEM437::BYTES_PER_CHAR;
	for (Uint8 yc = 0; yc < OEM437::CHAR_HEIGHT; yc++, charIndex++, screenIndex += (getScreenWidth() - OEM437::CHAR_WIDTH))
	{
		Uint8 charByte = SYSTEM::memory[SYSTEM::MEM_ASCII + charIndex];
		for (Uint8 xc = 0; xc < OEM437::CHAR_WIDTH; xc++, charByte = charByte >> 1)
		{
			if ((charByte & 1) != 0)
			{
				_graphicsMemory[screenIndex] = fgColor;
			}
			else if (bgColor != 255)
			{
				_graphicsMemory[screenIndex] = bgColor;
			}
			screenIndex++;
			//_graphicsMemory[screenIndex++] = ((charByte & 1) != 0) ? fgColor : bgColor;
		}
	}
}
*/

void CGA::GraphicsMode::drawChar(int x, int y, Uint8 fgColor, Uint8 bgColor, unsigned char ch)
{
	drawBitmap(x, y, fgColor, bgColor, (unsigned char *)(SYSTEM::memory + SYSTEM::MEM_ASCII + ch * OEM437::BYTES_PER_CHAR));
	/*
	Uint32 charIndex = ch * OEM437::BYTES_PER_CHAR;
	for (Uint8 yc = 0; yc < OEM437::CHAR_HEIGHT; yc++, charIndex++)
	{
		Uint8 charByte = SYSTEM::memory[SYSTEM::MEM_ASCII + charIndex];
		for (Uint8 xc = 0; xc < OEM437::CHAR_WIDTH; xc++, charByte = charByte >> 1)
		{
			if ((charByte & 1) != 0)
			{
				setPixel(x + xc, y + yc, fgColor);
			}
			else if (bgColor != 255)
			{
				setPixel(x + xc, y + yc, bgColor);
			}
		}
	}
	*/
}

void CGA::GraphicsMode::drawString(int x, int y, Uint8 fgColor, Uint8 bgColor, const char* text)
{
	for (; *text != 0; text++, x += OEM437::CHAR_WIDTH)
	{
		drawChar(x, y, fgColor, bgColor, *text);
	}
}

void CGA::GraphicsMode::clearGraphicsScreen(int color)
{
	for (Uint32 index = 0; index < getScreenWidth() * getScreenHeight(); index++)
	{
		_graphicsMemory[index] = color;
	}
}

void CGA::GraphicsMode::drawTestPattern1()
{
	drawFilledRect(0, getScreenWidth() - 1, 0, getScreenHeight() - 1, 4);

	for (Uint8 c = 0; c <= 15; c++)
	{
		Uint16 y = c * getScreenHeight() / 16;
		drawHLine(0, getScreenWidth() - 1, y, c);

		Uint16 x = c * getScreenWidth() / 16;
		drawVLine(x, 0, getScreenHeight() - 1, c);
	}
}

void CGA::GraphicsMode::drawTestPattern2()
{
	for (Uint8 c = 0; c <= 15; c++)
	{
		Uint16 x1 = c * getScreenWidth() / 16;
		Uint16 x2 = (c + 1) * getScreenWidth() / 16 - 1;
		drawFilledRect(x1, x2, 0, getScreenHeight() - 1, c);
	}
}
