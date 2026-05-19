#include "TextMode.h"

void CGA::TextMode::initialize()
{
	CGA::GraphicsMode::initialize();

	_textMemory = (uint8_t *)calloc(getTextRows() * getTextColumns() * getBytesPerChar() * getNumPages(), 1);
}

CGA::TextMode::~TextMode()
{
	if (_textMemory != NULL)
	{
		free(_textMemory);
		_textMemory = NULL;
	}
}

void CGA::TextMode::writeTestPattern()
{
	for (Uint8 column = 0; column < getTextColumns(); column++)
	{
		Uint8 color = column * 16 / getTextColumns();
		for (Uint8 row = 0; row < getTextRows(); row++)
		{
			writeChar(row, column, color << 4, 219);
		}
	}
}

void CGA::TextMode::clearTextScreen(int attr)
{
	for (Uint32 index = 0; index < getTextColumns() * getTextRows() * getBytesPerChar(); index += 2)
	{
		_textMemory[index] = 0;
		_textMemory[index + 1] = attr;
	}
}

void CGA::TextMode::writeChar(Uint8 row, Uint8 column, Uint8 attr, unsigned char ch)
{
	Uint32 index = row * getTextColumns() * getBytesPerChar() + column * getBytesPerChar();
	_textMemory[index] = ch;
	_textMemory[index + 1] = attr;
}

void CGA::TextMode::writeChar(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor, unsigned char ch)
{
	writeChar(row, column, (fgColor << 4) + bgColor, ch);
}

void CGA::TextMode::writeString(Uint8 row, Uint8 column, Uint8 attr, const char* text)
{
	for (; *text != 0; text++, column++)
	{
		writeChar(row, column, attr, *text);
	}
}

void CGA::TextMode::writeString(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor, const char* text)
{
	writeString(row, column, (fgColor << 4) + bgColor, text);
}

void CGA::TextMode::writeInteger(Uint8 row, Uint8 column, Uint8 attr, int value, int base, int width)
{
	char buffer[32] = { 0 };
	int numDigits = 0;

	while (true)
	{
		char digit = value % base;
		buffer[numDigits] = digit;
		value = (int)(value / base);
		numDigits++;
		if (value == 0)
		{
			break;
		}
	}
	while (numDigits < width)
	{
		buffer[numDigits] = 0;
		numDigits++;
	}
	for (int n = 0; n < numDigits; n++)
	{
		char digit = buffer[numDigits - n - 1];
		char ch = (digit < 10) ? (digit + '0') : (digit - 10 + 'A');
		writeChar(row, column + n, attr, ch);
	}
}

void CGA::TextMode::writeInteger(Uint8 row, Uint8 column, Uint8 fgColor, Uint8 bgColor, int value, int base, int width)
{
	writeInteger(row, column, (fgColor << 4) + bgColor, value, base, width);
}

bool CGA::TextMode::render()
{
	Uint32 index = _textPagingOffset;
	Uint32 maxIndex = index + (Uint32)getTextColumns() * (Uint32)getTextRows() * (Uint32)getBytesPerChar();
	Uint16 x = 0;
	Uint16 y = 0;

	for (; index < maxIndex; index += getBytesPerChar())
	{
		Uint8 ch = _textMemory[index];
		Uint8 attr = _textMemory[index + 1];

		if ((y == 19 * 8) && (x == 37 * 8))
		{
			int a = 0;
		}

		drawChar(x, y, attr, ch);

		x += OEM437::CHAR_WIDTH;
		if (x >= getTextColumns() * OEM437::CHAR_WIDTH)
		{
			x = 0;
			y += OEM437::CHAR_HEIGHT;
		}
	}

	return GraphicsMode::render();
}
