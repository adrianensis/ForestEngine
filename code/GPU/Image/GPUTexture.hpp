#pragma once

#include "Core/Minimal.hpp"
#include "Core/Font/Font.hpp"

#include "GPU/Image/GPUImage.h"
#include "GPU/Core/GPUDefinitions.h"
#include "Core/Image/ImageUtils.hpp"

enum class GPUTextureChannels
{
    SINGLE = 1,
    RGB = 3,
    RGBA = 4
};

class GPUTextureData
{
public:
    HashedString mPath;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
    bool mIsFont = false;
    FontData mFontData;
};

class GPUTexture
{
public:
    void init(Ptr<GPUContext> gpuContext, const GPUTextureData& gpuTextureData, u32 id);
    void terminate();
    
private:
	u32 mGPUTextureId = 0;
    TextureHandle mGPUTextureHandle = 0;
    ImageData mImageData;
	u32 mID = 0;
	GPUTextureData mTextureData;
    Ptr<GPUContext> mGPUContext;
    GPUImage mVulkanTextureImage;
    u32 mMipMapLevels = 1;

public:
    VkImageView mTextureImageView = VK_NULL_HANDLE;
    VkSampler mTextureSampler = VK_NULL_HANDLE;

public:
    GET(ID)
    GET(GPUTextureHandle)
};
REGISTER_CLASS(GPUTexture);
