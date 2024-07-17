#include "Graphics/Material/Texture.hpp"
#include "Core/Image/ImageUtils.hpp"

void Texture::enable(u32 textureUnit) const
{
    GET_SYSTEM(GPUInterface).enableTexture(mGPUTextureId, textureUnit, mTextureData.mStage);
}

void Texture::disable(u32 textureUnit) const
{
    GET_SYSTEM(GPUInterface).disableTexture(textureUnit, mTextureData.mStage);
}

void Texture::init(const TextureData& textureData, u32 id)
{
    mTextureData = textureData;
    mID = id;

    if(textureData.mIsFont)
    {
        mWidth = mTextureData.mFontData.mWidth;
        mHeight = mTextureData.mFontData.mHeight;

        mGPUTextureId = GET_SYSTEM(GPUInterface).createTexture1ByteChannel(mWidth, mHeight, nullptr);

        u32 texPos = 0;
        FOR_RANGE(c, 0, mTextureData.mFontData.mGlyphs.size())
        {
            GET_SYSTEM(GPUInterface).setSubTexture(mGPUTextureId, texPos, 0, mTextureData.mFontData.mGlyphs[c].mBitmapSize.x, mTextureData.mFontData.mGlyphs[c].mBitmapSize.y, GPUTexturePixelFormat::RED, GPUPrimitiveDataType::UNSIGNED_BYTE, mTextureData.mFontData.mGlyphs[c].mData);
            // Increase texture offset
            texPos += mTextureData.mFontData.mGlyphs[c].mBitmapSize.x /*+ 2*/;
        }
    }
    else
    {
        ImageData imageData;
        imageData = ImageUtils::loadImage(textureData.mPath);
        mWidth = imageData.mWidth;
        mHeight = imageData.mHeight;
        mGPUTextureId = GET_SYSTEM(GPUInterface).createTexture(GPUTextureFormat::RGBA8, mWidth, mHeight, true);
        GET_SYSTEM(GPUInterface).setTextureData(mGPUTextureId, mWidth, mHeight, GPUTexturePixelFormat::RGBA, GPUPrimitiveDataType::UNSIGNED_BYTE, true, imageData.mData);
        CHECK_MSG(imageData.mData, "Error loading image " + mTextureData.mPath.get());
        ImageUtils::freeImage(imageData);
    }

    // Retrieve the texture handle after we finish creating the texture
    mGPUTextureHandle = GET_SYSTEM(GPUInterface).getTextureHandle(mGPUTextureId);
    CHECK_MSG(mGPUTextureHandle > 0, "TextureHandle error!");
    GET_SYSTEM(GPUInterface).makeTextureResident(mGPUTextureHandle, true);
}

void Texture::terminate() 
{
    if(mGPUTextureId > 0)
    {
        GET_SYSTEM(GPUInterface).deleteTexture(mGPUTextureId);
        mGPUTextureId = 0;
        mGPUTextureHandle = 0;
    }
}