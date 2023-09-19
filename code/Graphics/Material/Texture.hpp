#pragma once

#include "Core/Module.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

class TextureFontGlyph
{
public:
    Vector2 mAdvance;
    Vector2 mBitmapSize;
    Vector2 mBitmapTopLeft;
    f32 xOffset;
    byte* mData = nullptr;
};

class TextureData
{
public:
    std::string mPath;
    bool mCreateMipMap = true;
    std::vector<TextureFontGlyph> mFontGlyphs;
    u32 mFontWidth = 0;
	u32 mFontHeight = 0;
};

class Texture: public ObjectBase
{
    GENERATE_METADATA(Texture)
public:
    ~Texture() override;

    virtual void init(const TextureData& textureData) {};
    void bind() const;

protected:
	u32 mTextureId = 0;
	u32 mWidth = 0;
	u32 mHeight = 0;
	TextureData mTextureData;
};

class TextureImage: public Texture
{
    GENERATE_METADATA(TextureImage)
public:
    void init(const TextureData& textureData);
};

class TextureFont: public Texture
{
    GENERATE_METADATA(TextureFont)
public:
    void init(const TextureData& textureData);
};
