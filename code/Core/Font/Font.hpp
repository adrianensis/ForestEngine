#pragma once

#include "Core/Maths/Geometry.hpp"
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

class FontGlyphMetricsData
{
public:
    Vector2 mSize;
    Vector2 mBoundingBoxMin;
    Vector2 mBoundingBoxMax;
    Vector2 mHoriBearing;
    Vector2 mVertBearing;
};

class FontGlyphData
{
public:
    void freeBuffer();

public:
    Vector2 mAdvance;
    Vector2 mBitmapSize;
    Vector2 mBitmapTopLeft;
    FontGlyphMetricsData mMetrics;
    Rectangle mTextureRegion;
    byte* mData = nullptr;
};

class FontData
{
public:
	void loadFont(FontsLibrary& fontsLibrary, HashedString fontFile, u32 fontSize);
	void freeGlyphsBuffers();
public:
    inline static const u32 MAX_GLYPHS = 128;
public:
    FT_Face mFreeTypeFace;
    HashedString mPath;
    u32 mWidth;
    u32 mHeight;
    std::array<FontGlyphData, FontData::MAX_GLYPHS> mGlyphs;
};
