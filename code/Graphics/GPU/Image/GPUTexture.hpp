#pragma once

#include "Core/Minimal.hpp"
#include "Core/Font/Font.hpp"

#include "Graphics/GPU/Image/GPUImage.h"
#include "Core/Object/ObjectBase.hpp"

class GPUTextureData
{
public:
    HashedString mPath;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
    bool mIsFont = false;
    FontData mFontData;
};

class GPUTexture: public ObjectBase, public IPoolable
{
public:
    void init(Ptr<GPUContext> gpuContext, const GPUTextureData& gpuTextureData, u32 id);
    void terminate();
    virtual void onPoolFree() override { terminate(); };
    void enable(u32 textureUnit) const;
    void disable(u32 textureUnit) const;

private:
    bool initializeTextureImage();
    bool initializeTextureImageView();
    bool initializeTextureSampler();
    bool generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const;
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

private:
	u32 mGPUTextureId = 0;
    TextureHandle mGPUTextureHandle = 0;
	u32 mWidth = 0;
	u32 mHeight = 0;
	u32 mID = 0;
	GPUTextureData mTextureData;
    Ptr<GPUContext> mGPUContext;
    GPUImage* vulkanTextureImage;
    uint32_t mipLevels = 0;

public:
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;

public:
    GET(ID)
    GET(GPUTextureHandle)
};
REGISTER_CLASS(GPUTexture);
