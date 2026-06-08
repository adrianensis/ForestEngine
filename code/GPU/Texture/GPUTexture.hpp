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
    std::string mPath;
    bool mIsLinearData = false;
    bool mIsFont = false;
    Font::FontData mFontData;
    bool operator==(const GPUTextureData& other) const { return this->mPath == other.mPath; }
};


class GPUTexture;
class GPUTextureHandle
{
public:
    GPUTextureHandle(){};
    GPU::u32 mSlot = 0;
    bool mIsValid = false;
#ifdef ENGINE_BUILD_DEBUG
    GPUTexture* mDebugGPUTexture = nullptr;
#endif
};

class GPUTexture
{
public:
    void init(GPUContext* gpuContext, const GPUTextureData& gpuTextureData);
    void terminate();
    bool isValid() const { return mValid; }
    
private:
    Image::ImageData mImageData;
	GPUTextureData mTextureData;
    GPUContext* mGPUContext = nullptr;
    GPUImage mVulkanTextureImage;
    GPU::u32 mMipMapLevels = 1;
    VkFormat mFormat = VK_FORMAT_R8G8B8A8_SRGB;
    bool mValid = false;

public:
    VkImageView mTextureImageView = VK_NULL_HANDLE;
    VkSampler mTextureSampler = VK_NULL_HANDLE;

public:
    auto getTextureData() const { return mTextureData; }
};

