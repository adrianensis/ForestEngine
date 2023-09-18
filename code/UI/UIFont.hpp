#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/Material.hpp"

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

class UIFont: public ObjectBase
{
    GENERATE_METADATA(UIFont)
public:
    void init(UIFontsManager& fontsManager, const std::string& fontFile);

private:
    static void vertical_flip(void *image, int w, int h, int bytes_per_pixel);
private:
    FT_Face mFreeTypeFace;
    std::string mPath;
    u32 mWidth;
    u32 mHeight;
    inline static const u32 MAX_GLYPHS = 128;
    std::array<TextureFontGlyph, MAX_GLYPHS> mGlyphs;
    Ptr<const MaterialFont> mFontMaterial;
public:
    CRGET(FreeTypeFace)
    GET(FontMaterial)
    GET(Width)
    GET(Height)
    CRGET(Glyphs)
};