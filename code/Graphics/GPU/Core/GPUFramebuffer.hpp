#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Core/GPUBuiltIn.hpp"
#include "Graphics/GPU/Core/GPUSwapChain.h"
#include "Graphics/GPU/Core/GPURenderPass.h"

class GPUFramebufferData
{
public:
    void set(const std::unordered_set<GPUFramebufferAttachmentType>& attachments, u32 width, u32 height)
    {
        mAttachments = attachments;
        mWidth = width;
        mHeight = height;
        mIsValid = true;
    }
    bool isValid() const { return mIsValid; }

    std::unordered_set<GPUFramebufferAttachmentType> mAttachments;
    u32 mWidth = 0;
    u32 mHeight = 0;
    
private:
    bool mIsValid = false;
};

class GPUFramebufferAttachment
{
public:
    GPUFramebufferAttachmentType mGPUFramebufferAttachmentType = GPUFramebufferAttachmentType::NONE;
    u32 mAttachmentID = 0;
};

class GPUFramebuffer
{
public:
    void init(const GPUFramebufferData& framebufferData);
    Vector4 readPixel(u32 x, u32 y, GPUFramebufferAttachmentType attachmentType) const;
    void enable(GPUFramebufferOperationType op);
    void disable(GPUFramebufferOperationType op);

    const VkFramebuffer getFramebuffer() const;
    bool initialize(Ptr<GPUContext> gpuContext, GPURenderPass* vulkanRenderPasssss, VkImageView colorImageView, VkImageView depthImageView, VkImageView swapChainImageView);
    void terminate();

private:
    u32 mFramebufferId = 0;
    std::unordered_map<GPUFramebufferAttachmentType, GPUFramebufferAttachment> mAttachments;
    GPUFramebufferData mFramebufferData;

    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

    GPURenderPass* vulkanRenderPass;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    Ptr<GPUContext> mGPUContext;

public:
    GET(FramebufferId)
    CRGET(FramebufferData)
    CRGET(Attachments)
};
