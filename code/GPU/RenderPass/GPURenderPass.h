#pragma once

#include "GPU/Shader/GPUShaderModule.h"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImage.h"

enum class GPUAttachmentLoadOp
{
    LOAD = VK_ATTACHMENT_LOAD_OP_LOAD,
    CLEAR = VK_ATTACHMENT_LOAD_OP_CLEAR,
    DONT_CARE = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
};
enum class GPUAttachmentStoreOp
{
    STORE = VK_ATTACHMENT_STORE_OP_STORE,
    DONT_CARE = VK_ATTACHMENT_STORE_OP_DONT_CARE,
    NONE = VK_ATTACHMENT_STORE_OP_NONE,
    NONE_KHR = VK_ATTACHMENT_STORE_OP_NONE_KHR,
    NONE_QCOM = VK_ATTACHMENT_STORE_OP_NONE_QCOM,
    NONE_EXT = VK_ATTACHMENT_STORE_OP_NONE_EXT,
};

class GPUAttachmentData
{
public:
    GPUAttachmentLoadOp mGPUAttachmentLoadOp = GPUAttachmentLoadOp::CLEAR;
    GPUAttachmentStoreOp mGPUAttachmentStoreOp = GPUAttachmentStoreOp::DONT_CARE;
    Ptr<GPUImage> mGPUImage;
};

class GPURenderPassData
{
public:
    GPUAttachmentData mColorAttachment;
    GPUAttachmentData mDepthStencilAttachment;
    VkSampleCountFlagBits mSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT;
    bool mIsResolvePass = false;
};

class GPURenderPass : public EnableWeakPtrToThis
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPURenderPass();
    bool init(Ptr<GPUContext> gpuContext, const GPURenderPassData& gpuRenderPassData);
    void terminate();
    void begin();
    void end();
    void onResize();
private:

    bool initializeFramebuffers();

public:
    Ptr<GPUContext> mGPUContext;
private:
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    std::vector<GPUFramebuffer> framebuffers;

    GPURenderPassData mGPURenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
public:
    CRGET(RenderPass)
    CRGET(GPURenderPassData)
    RGET(OutputGPUFramebuffer)
};



