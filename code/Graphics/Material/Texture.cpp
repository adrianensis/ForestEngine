#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "png.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::~Texture() 
{
    GET_SYSTEM(GPUInterface).deleteTexture(mTextureId);
}

void Texture::bind() const
{
    GET_SYSTEM(GPUInterface).enableTexture(mTextureId);
}

void TextureImage::init(const TextureData& textureData)
{
	mTextureData = textureData;
    byte* data = loadImage();
    mTextureId = GET_SYSTEM(GPUInterface).createTexture(mWidth, mHeight, GL_RGBA, data, textureData.mCreateMipMap);
    CHECK_MSG(data, "Error loading image " + mTextureData.mPath);
    stbi_image_free(data);
}

byte* TextureImage::loadImage()
{
	i32 width, height, original_number_channels;
	stbi_set_flip_vertically_on_load(true);

	if(mTextureData.mPath.find("\\") != std::string::npos)
	{
		mTextureData.mPath.replace(mTextureData.mPath.find("\\"), 1, "/");
	}

	byte* data = stbi_load(mTextureData.mPath.c_str(), &width, &height, &original_number_channels, STBI_rgb_alpha);
	mWidth = width;
    mHeight = height;
    return data;
}

void TextureFont::init(const TextureData& textureData)
{
    mTextureData = textureData;
    mWidth = mTextureData.mFontWidth;
    mHeight = mTextureData.mFontHeight;
    mTextureId = GET_SYSTEM(GPUInterface).createTextureFont(mWidth, mHeight, GL_RED, nullptr);

    GET_SYSTEM(GPUInterface).enableTexture(mTextureId);

    u32 texPos = 0;
    FOR_RANGE(c, 0, 1)
    {
        // GET_SYSTEM(GPUInterface).subTexture(texPos, 0, 1, mTextureData.mFontGlyphs[c].mBitmapSize.y, GL_RED, nullptr);
        GET_SYSTEM(GPUInterface).subTexture(texPos, 0, mTextureData.mFontGlyphs[c].mBitmapSize.x, mTextureData.mFontGlyphs[c].mBitmapSize.y, GL_RED, mTextureData.mFontGlyphs[c].mData);
        // GET_SYSTEM(GPUInterface).subTexture(texPos, 0, 1, mTextureData.mFontGlyphs[c].mBitmapSize.y, GL_RED, nullptr);

        // Increase texture offset
        texPos += mTextureData.mFontGlyphs[c].mBitmapSize.x /*+ 2*/;
    }
    GET_SYSTEM(GPUInterface).disableTexture();
}