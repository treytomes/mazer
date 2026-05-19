#pragma once

#include "../SYSTEM/SYSTEM.h"
#include "TextMode.h"

namespace CGA
{
	/*
	 * Text mode, 40x30 characters, 256 colors.
	 */
	class MODE_A : public TextMode
	{
	public:
		void initialize()
		{
			TextMode::initialize();

			SDL_Color colors[] = {
				SYSTEM::convertHtmlColor((int)COLORS::BLACK), SYSTEM::convertHtmlColor((int)COLORS::BLUE), SYSTEM::convertHtmlColor((int)COLORS::GREEN), SYSTEM::convertHtmlColor((int)COLORS::CYAN),
				SYSTEM::convertHtmlColor((int)COLORS::RED), SYSTEM::convertHtmlColor((int)COLORS::MAGENTA), SYSTEM::convertHtmlColor((int)COLORS::BROWN), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_GRAY),
				SYSTEM::convertHtmlColor((int)COLORS::DARK_GRAY), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_BLUE), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_GREEN), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_CYAN),
				SYSTEM::convertHtmlColor((int)COLORS::LIGHT_RED), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_MAGENTA), SYSTEM::convertHtmlColor((int)COLORS::YELLOW), SYSTEM::convertHtmlColor((int)COLORS::WHITE)
			};

			setPalette(colors);

			clearTextScreen(0);

			_isModeSet = true;
		}

		int getTextColumns() { return 40; }
		int getTextRows() { return 30; }
		int getBitsPerPixel() { return 8; }
		int getNumPages() { return 8; }
	};

	/*
	 * Graphics mode, 320x240 pixels, 256 colors.
	 */
	class MODE_B : public GraphicsMode
	{
	public:
		void initialize()
		{
			GraphicsMode::initialize();

			SDL_Color colors[] = {
				SYSTEM::convertHtmlColor((int)COLORS::BLACK), SYSTEM::convertHtmlColor((int)COLORS::BLUE), SYSTEM::convertHtmlColor((int)COLORS::GREEN), SYSTEM::convertHtmlColor((int)COLORS::CYAN),
				SYSTEM::convertHtmlColor((int)COLORS::RED), SYSTEM::convertHtmlColor((int)COLORS::MAGENTA), SYSTEM::convertHtmlColor((int)COLORS::BROWN), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_GRAY),
				SYSTEM::convertHtmlColor((int)COLORS::DARK_GRAY), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_BLUE), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_GREEN), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_CYAN),
				SYSTEM::convertHtmlColor((int)COLORS::LIGHT_RED), SYSTEM::convertHtmlColor((int)COLORS::LIGHT_MAGENTA), SYSTEM::convertHtmlColor((int)COLORS::YELLOW), SYSTEM::convertHtmlColor((int)COLORS::WHITE)
			};

			setPalette(colors);

			clearGraphicsScreen(0);

			_isModeSet = true;
		}

		int getScreenWidth() { return 320; }
		int getScreenHeight() { return 240; }
		int getBitsPerPixel() { return 8; }
		int getNumPages() { return 4; }
	};
}
