#pragma once

#include "Engine/Core.hpp"
#include "Image/Font/Font.hpp"

#include "GPU/Image/GPUImage.h"
#include "GPU/Core/GPUDefinitions.h"
#include "Image/ImageUtils.hpp"

enum class GPUTextureChannels
{
    SINGLE = 1,
    RGB = 3,
    RGBA = 4
};

class GPUTextureData
{
public:
    Core::HashedString mPath;
    bool mIsFont = false;
    Font::FontData mFontData;
};

class GPUTexture
{
public:
    void init(Core::Ptr<GPUContext> gpuContext, const GPUTextureData& gpuTextureData, Core::u32 id);
    void terminate();
    
private:
	Core::u32 mGPUTextureId = 0;
    TextureHandle mGPUTextureHandle = 0;
    Image::ImageData mImageData;
	Core::u32 mID = 0;
	GPUTextureData mTextureData;
    Core::Ptr<GPUContext> mGPUContext;
    GPUImage mVulkanTextureImage;
    Core::u32 mMipMapLevels = 1;

public:
    VkImageView mTextureImageView = VK_NULL_HANDLE;
    VkSampler mTextureSampler = VK_NULL_HANDLE;

public:
    GET(ID)
    GET(GPUTextureHandle)
};
REGISTER_CLASS(GPUTexture);
