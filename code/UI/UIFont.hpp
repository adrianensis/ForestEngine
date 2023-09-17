#pragma once

#include "Core/Module.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

class UIFont;

class UIFontsManager: public ObjectBase
{
    GENERATE_METADATA(UIFontsManager)
public:
    void init();
    void terminate();
    Ptr<UIFont> loadFont(const std::string& fontFile);

private:
    FT_Library mFreeTypeLibrary;
    std::unordered_map<std::string, OwnerPtr<UIFont>> mFontsMap;
public:
    CRGET(FreeTypeLibrary)
};

class UIFontGlyph
{
public:
    Vector2 mAdvance;
    Vector2 mBitmapSize;
    Vector2 mBitmapTopLeft;
    f32 xOffset;
};

class UIFont: public ObjectBase
{
    GENERATE_METADATA(UIFont)
public:
    ~UIFont();
    void init(UIFontsManager& fontsManager, const std::string& fontFile);

private:
    FT_Face mFreeTypeFace;
    std::string mPath;
    u32 mWidth;
    u32 mHeight;
    std::array<UIFontGlyph, 128> mGlyphs;
public:
    CRGET(FreeTypeFace)
};

