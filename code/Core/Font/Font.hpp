#pragma once

#include "Core/Object/ObjectBase.hpp"
#include "Core/Maths/Vector2.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

class FontsLibrary
{
public:
    void init();
    void terminate();
private:
    FT_Library mFreeTypeLibrary;
public:
    RGET(FreeTypeLibrary)
};

class FontGlyphData
{
public:
    void freeBuffer();

public:
    Vector2 mAdvance;
    Vector2 mBitmapSize;
    Vector2 mBitmapTopLeft;
    f32 xOffset;
    byte* mData = nullptr;
};

class FontData
{
public:
	void loadFont(FontsLibrary& fontsLibrary, const std::string& fontFile);
	void freeGlyphsBuffers();
public:
    inline static const u32 MAX_GLYPHS = 128;
public:
    FT_Face mFreeTypeFace;
    std::string mPath;
    u32 mWidth;
    u32 mHeight;
    std::array<FontGlyphData, FontData::MAX_GLYPHS> mGlyphs;
};
