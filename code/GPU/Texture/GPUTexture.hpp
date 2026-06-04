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
    GPUTextureHandle(const Core::Slot& slot) : mSlot(slot){};
    Core::Slot mSlot;
#ifdef ENGINE_BUILD_DEBUG
    GPUTexture* mDebugGPUTexture = nullptr;
#endif
};

class GPUTexture
{
public:
    void init(GPUContext* gpuContext, const GPUTextureData& gpuTextureData, Core::Slot slot);
    void terminate();
    bool isValid() const { return mSlot.isValid(); }
    
private:
    Image::ImageData mImageData;
	Core::Slot mSlot;
	GPUTextureData mTextureData;
    GPUContext* mGPUContext = nullptr;
    GPUImage mVulkanTextureImage;
    GPU::u32 mMipMapLevels = 1;
    VkFormat mFormat = VK_FORMAT_R8G8B8A8_SRGB;

public:
    VkImageView mTextureImageView = VK_NULL_HANDLE;
    VkSampler mTextureSampler = VK_NULL_HANDLE;

public:
    GET(Slot)
    GET(TextureData)
};

