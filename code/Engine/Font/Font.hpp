#pragma once

#include "Core/Core.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

NS_BEGIN(Font)
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

class FontGlyphMetricsData
{
public:
    Maths::Vector2 mSize;
    Maths::Vector2 mBoundingBoxMin;
    Maths::Vector2 mBoundingBoxMax;
    Maths::Vector2 mHoriBearing;
    Maths::Vector2 mVertBearing;
};

class FontGlyphData
{
public:
    void freeBuffer();

public:
    Maths::Vector2 mAdvance;
    Maths::Vector2 mBitmapSize;
    Maths::Vector2 mBitmapTopLeft;
    FontGlyphMetricsData mMetrics;
    Maths::Cube mTextureRegion;
    Core::byte* mData = nullptr;
};

class FontData
{
public:
	void loadFont(FontsLibrary& fontsLibrary, Core::HashedString fontFile, Core::u32 fontSize);
	void freeGlyphsBuffers();
public:
    inline static const Core::u32 MAX_GLYPHS = 128;
public:
    FT_Face mFreeTypeFace;
    Core::HashedString mPath;
    Core::u32 mWidth;
    Core::u32 mHeight;
    std::array<FontGlyphData, FontData::MAX_GLYPHS> mGlyphs;
    Core::byte* mGlyphAtlasData = nullptr;
};
NS_END