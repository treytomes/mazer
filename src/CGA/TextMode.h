#pragma once

#include "GraphicsMode.h"
#include "../OEM437.h"

namespace CGA
{
	class TextMode : public GraphicsMode
	{
	protected:
		// TODO: Does this really need to be 32-bit?
		Uint32 _textPagingOffset = 0;

		// Text operations will write to this array.
		uint8_t* _textMemory;

	public:
		virtual void initialize();

		~TextMode();

		virtual int getTextColumns() = 0;
		virtual int getTextRows() = 0;
		inline int getScreenWidth() { return getTextColumns() * OEM437::CHAR_WIDTH; }
		inline int getScreenHeight() { return getTextRows() * OEM437::CHAR_HEIGHT; }
		virtual int getNumPages() = 0;
		virtual inline int getBytesPerChar() { return 2; }

		void clearTextScreen(int attr);

		inline Uint8 getAttr(Uint8 row, Uint8 column) { return _textMemory[row * getTextColumns() * getBytesPerChar() + column * getBytesPerChar() + 1]; }
		inline void setAttr(Uint8 row, Uint8 column, Uint8 attr) { _textMemory[row * getTextColumns() * getBytesPerChar() + column * getBytesPerChar() + 1] = attr; }
		inline void setAttr(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor) { setAttr(row, column, (fgColor << 4) + bgColor); }
		inline void invertAttr(Uint8 row, Uint8 column) { Uint8 attr = getAttr(row, column); setAttr(row, column, attr & 0xF, attr >> 4); }
		void writeChar(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor, unsigned char ch);
		void writeChar(Uint8 row, Uint8 column, Uint8 attr, unsigned char ch);
		void writeString(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor, const char* text);
		void writeString(Uint8 row, Uint8 column, Uint8 attr, const char* text);
		void writeInteger(Uint8 row, Uint8 column, Uint8 attr, int value, int base = 10, int width = 0);
		void writeInteger(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor, int value, int base = 10, int width = 0);

		void writeTestPattern();

		bool render();
	};
}