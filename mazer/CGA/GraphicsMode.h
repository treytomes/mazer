#pragma once

#include <SDL.h>
#include "../SYSTEM/SYSTEM.h"

namespace CGA
{
	class GraphicsMode
	{
	private:
		/*
		* Draw all 8 quadrants simultaneously.
		*/
		inline void drawCirclePoints(int xc, int yc, int x, int y, Uint8 color)
		{
			setPixel(xc + x, yc + y, color);
			setPixel(xc + x, yc - y, color);
			setPixel(xc - x, yc + y, color);
			setPixel(xc - x, yc - y, color);
			setPixel(xc + y, yc + x, color);
			setPixel(xc + y, yc - x, color);
			setPixel(xc - y, yc + x, color);
			setPixel(xc - y, yc - x, color);
		}

	protected:
		// The drawing surface must be converted before being scaled onto the screen.
		SDL_Surface* _convertedSurface = NULL;

		// The image we will load and show on the screen
		SDL_Surface* _drawingSurface = NULL;

		bool _isModeSet = false;

		// Graphics operations will write to this array.
		uint8_t* _graphicsMemory;

	//protected:
	//	void drawCharInternal(Uint32 screenIndex, Uint8 fgColor, Uint8 bgColor, unsigned char ch);

	public:
		GraphicsMode();
		~GraphicsMode();

		virtual void initialize();

		bool isModeSet();
		virtual int getScreenWidth() = 0;
		virtual int getScreenHeight() = 0;
		virtual int getBitsPerPixel() = 0;

		inline Uint8 getPixel(int x, int y) { return _graphicsMemory[y * getScreenWidth() + x]; }
		inline void setPixel(int x, int y, Uint8 color)
		{
			if ((x < 0) || (x >= getScreenWidth()) || (y < 0) || (y >= getScreenHeight())) { return; }
			_graphicsMemory[y * getScreenWidth() + x] = color;
		}

		// "draw" methods draw to the screen.  "write" methods write to text memory.

		virtual void drawCircle(int xc, int yc, int r, Uint8 color);
		virtual void drawLine(int x1, int y1, int x2, int y2, Uint8 color);
		virtual void drawHLine(int x1, int x2, int y, Uint8 color);
		virtual void drawVLine(int x, int y1, int y2, Uint8 color);
		virtual void drawFilledRect(int x1, int x2, int y1, int y2, Uint8 color);

		virtual void drawBitmap(int x, int y, Uint8 fgColor, Uint8 bgColor, unsigned char *bitmap);

		//inline void drawChar(Uint16 x, Uint16 y, Uint8 fgColor, Uint8 bgColor, unsigned char ch) { drawCharInternal(y * getScreenWidth() + x, fgColor, bgColor, ch); }
		virtual void drawChar(int x, int y, Uint8 fgColor, Uint8 bgColor, unsigned char ch);
		inline void drawChar(int x, int y, Uint8 attr, unsigned char ch) { drawChar(x, y, attr >> 4, attr & 0xF, ch); }


		virtual void drawString(int x, int y, Uint8 fgColor, Uint8 bgColor, const char* text);
		inline void drawString(int x, int y, Uint8 attr, const char* text) { drawString(x, y, attr >> 4, attr & 0xF, text); }

		// Draw a series of squares to the screen, to show off the aspect ratio.
		void drawTestPattern1();

		// Draw a series of colored bars to the screen, to show off the palette.
		void drawTestPattern2();

		void clearGraphicsScreen(int color);

		bool setPalette(SDL_Color* colors);

		// Draw this screen to the window.
		virtual bool render();
	};
}