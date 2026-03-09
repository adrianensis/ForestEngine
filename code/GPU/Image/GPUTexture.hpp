#pragma once

#include "Image/Font/Font.hpp"

#include "GPU/Image/GPUImage.h"
#include "GPU/Core/GPUDefinitions.h"
#include "Image/ImageUtils.hpp"

enum class GPUTextureChannels
{
    ONE = 1,
    THREE = 3,
    FOUR = 4
};

class GPUTextureData
{
public:
    Core::HashedString mPath;
    bool mIsFont = false;
    bool mIsLinearData = false;
    Font::FontData mFontData;
};

class GPUTexture
{
public:
    void init(GPUContext* gpuContext, const GPUTextureData& gpuTextureData, Core::u32 id);
    void terminate();
    
private:
    Image::ImageData mImageData;
	Core::u32 mID = 0;
	GPUTextureData mTextureData;
    GPUContext* mGPUContext = nullptr;
    GPUImage mVulkanTextureImage;
    Core::u32 mMipMapLevels = 1;
    VkFormat mFormat = VK_FORMAT_R8G8B8A8_SRGB;

public:
    VkImageView mTextureImageView = VK_NULL_HANDLE;
    VkSampler mTextureSampler = VK_NULL_HANDLE;

public:
    GET(ID)
    GET(TextureData)
};

