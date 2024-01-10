#include "Graphics/Material/Texture.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

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
    ImageData imageData;
    imageData = ImageUtils::loadImage(textureData.mPath);
    mWidth = imageData.mWidth;
    mHeight = imageData.mHeight;
    mTextureId = GET_SYSTEM(GPUInterface).createTexture(GPUTextureFormat::RGBA, mWidth, mHeight, GPUTexturePixelFormat::RGBA, imageData.mData, textureData.mCreateMipMap);
    CHECK_MSG(imageData.mData, "Error loading image " + mTextureData.mPath);
    ImageUtils::freeImage(imageData);
}

void TextureFont::init(const TextureData& textureData)
{
    mTextureData = textureData;
    mWidth = mTextureData.mFontData.mWidth;
    mHeight = mTextureData.mFontData.mHeight;
    mTextureId = GET_SYSTEM(GPUInterface).createTextureFont(GPUTextureFormat::RED, mWidth, mHeight, GPUTexturePixelFormat::RED, nullptr);

    GET_SYSTEM(GPUInterface).enableTexture(mTextureId);

    u32 texPos = 0;
    FOR_RANGE(c, 0, mTextureData.mFontData.mGlyphs.size())
    {
        // GET_SYSTEM(GPUInterface).subTexture(texPos, 0, 1, mTextureData.mFontGlyphs[c].mBitmapSize.y, GL_RED, nullptr);
        GET_SYSTEM(GPUInterface).subTexture(texPos, 0, mTextureData.mFontData.mGlyphs[c].mBitmapSize.x, mTextureData.mFontData.mGlyphs[c].mBitmapSize.y, GPUTextureFormat::RED, mTextureData.mFontData.mGlyphs[c].mData);
        // GET_SYSTEM(GPUInterface).subTexture(texPos, 0, 1, mTextureData.mFontGlyphs[c].mBitmapSize.y, GL_RED, nullptr);

        // Increase texture offset
        texPos += mTextureData.mFontData.mGlyphs[c].mBitmapSize.x /*+ 2*/;
    }
    GET_SYSTEM(GPUInterface).disableTexture();
}