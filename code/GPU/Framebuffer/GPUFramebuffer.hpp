#pragma once

#include "Core/Core.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Core/GPUDefinitions.h"
#include "GPU/Image/GPUImage.h"

class GPURenderPass;

class GPUFramebufferData
{
public:
    void set(const std::unordered_set<GPUFramebufferAttachmentType>& attachments, GPU::u32 width, GPU::u32 height)
    {
        mAttachments = attachments;
        mWidth = width;
        mHeight = height;
        mIsValid = true;
    }
    bool isValid() const { return mIsValid; }

    std::unordered_set<GPUFramebufferAttachmentType> mAttachments;
    GPU::u32 mWidth = 0;
    GPU::u32 mHeight = 0;
    VkSampleCountFlagBits mSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT;
    bool mIsResolveFramebuffer = false;
    GPU::u32 mSwapchainIndex = 0;
    GPUImage* mColorImage = nullptr;
    
private:
    bool mIsValid = false;
};

class GPUFramebufferAttachment
{
public:
    GPUFramebufferAttachmentType mGPUFramebufferAttachmentType = GPUFramebufferAttachmentType::NONE;
    GPU::u32 mAttachmentID = 0;
};

class GPUFramebuffer
{
public:
    bool init(GPUContext* gpuContext, const GPUFramebufferData& framebufferData);
    Maths::Vector4 readPixel(GPU::u32 x, GPU::u32 y, GPUFramebufferAttachmentType attachmentType) const;
    void enable(GPUFramebufferOperationType op);
    void disable(GPUFramebufferOperationType op);

    const VkFramebuffer getFramebuffer() const;
    void terminate();

private:
    bool initializeColorResources();
    bool initializeDepthResources();

private:
    GPU::u32 mFramebufferId = 0;
    std::unordered_map<GPUFramebufferAttachmentType, GPUFramebufferAttachment> mAttachments;
    GPUFramebufferData mFramebufferData;

    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    GPUContext* mGPUContext = nullptr;

    GPUImage vulkanDepthImage;
    VkImageView depthImageView = VK_NULL_HANDLE;
    VkImageView colorImageView = VK_NULL_HANDLE;
public:
    GPUImage vulkanColorImage;
    GET(FramebufferId)
    CRGET(FramebufferData)
    CRGET(Attachments)
};
