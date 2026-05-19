#pragma once

#include "CGA/GraphicsMode.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>

// Minimal concrete GraphicsMode that never touches SDL.
// Overrides all virtual draw methods to record calls instead of rendering.
// _graphicsMemory is allocated so any code that calls setPixel won't crash.
class MockGraphicsMode : public CGA::GraphicsMode
{
public:
    static const int W = 320;
    static const int H = 240;

    struct StringCall { int x, y; std::string text; };
    struct CharCall   { int x, y; unsigned char ch; };
    struct RectCall   { int x1, x2, y1, y2; };
    struct HLineCall  { int x1, x2, y; };
    struct VLineCall  { int x, y1, y2; };

    std::vector<StringCall> strings;
    std::vector<CharCall>   chars;
    std::vector<RectCall>   rects;
    std::vector<HLineCall>  hlines;
    std::vector<VLineCall>  vlines;

    MockGraphicsMode()
    {
        _graphicsMemory = static_cast<uint8_t*>(calloc(W * H, 1));
    }

    ~MockGraphicsMode()
    {
        free(_graphicsMemory);
        _graphicsMemory = nullptr;
    }

    int getScreenWidth()  override { return W; }
    int getScreenHeight() override { return H; }
    int getBitsPerPixel() override { return 8; }

    void drawString(int x, int y, Uint8 /*fgColor*/, Uint8 /*bgColor*/, const char* text) override
    {
        strings.push_back({x, y, text ? text : ""});
    }

    void drawChar(int x, int y, Uint8 /*fgColor*/, Uint8 /*bgColor*/, unsigned char ch) override
    {
        chars.push_back({x, y, ch});
    }

    void drawFilledRect(int x1, int x2, int y1, int y2, Uint8 /*color*/) override
    {
        rects.push_back({x1, x2, y1, y2});
    }

    void drawHLine(int x1, int x2, int y, Uint8 /*color*/) override
    {
        hlines.push_back({x1, x2, y});
    }

    void drawVLine(int x, int y1, int y2, Uint8 /*color*/) override
    {
        vlines.push_back({x, y1, y2});
    }

    bool render() override { return true; }

    void clear()
    {
        strings.clear();
        chars.clear();
        rects.clear();
        hlines.clear();
        vlines.clear();
    }

    bool hasString(const std::string& substr) const
    {
        for (auto& s : strings)
            if (s.text.find(substr) != std::string::npos) return true;
        return false;
    }
};
