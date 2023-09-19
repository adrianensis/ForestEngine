#pragma once

#include "Core/Module.hpp"

class TextureData
{
public:
    std::string mPath;
    bool mCreateMipMap = true;
    FontData mFontData;
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
    virtual void init(const TextureData& textureData) override;
};

class TextureFont: public Texture
{
    GENERATE_METADATA(TextureFont)
public:
    virtual void init(const TextureData& textureData) override;
};
