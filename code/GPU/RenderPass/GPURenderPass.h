#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImage.h"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"

class GPUSkeletalAnimationManager;

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
    GPUImage* mOutputGPUImage = nullptr;
    bool mUseDefaultOutput = true;
};

class GPURenderPassOutputData
{
public:
    GPUImage* mColorGPUImage = nullptr;
    GPUImage* mDepthGPUImage = nullptr;
    Core::u32 mResolveSwapchainImageIndex = 0;
};

class GPURenderPass;
class GPURenderPassDependency
{
public:
    Core::HashedString mSamplerName;
    GPUFramebufferAttachmentType mAttachmentType;
    Core::WeakPtr<GPURenderPass> mRenderPass;
};

class GPURenderPassData
{
public:
    GPUAttachmentData mColorAttachment;
    GPUAttachmentData mDepthStencilAttachment;
    VkSampleCountFlagBits mSampleCountFlagBits = VK_SAMPLE_COUNT_1_BIT;
    bool mIsResolvePass = false;
    Maths::GeometricSpace mGeometricSpace = Maths::GeometricSpace::WORLD;
    std::vector<GPURenderPassDependency> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
};

class GPURenderPass
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

public:
    GPURenderPass();
    virtual ~GPURenderPass() = default;
    // TODO: refactor so many arguments
    void init(GPUContext* gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager, Core::WeakPtr<GPUUniformBuffersContainer> globalGPUUniformBuffersContainer, const GPURenderPassData& gpuRenderPassData,
        GPUSkeletalAnimationManager* gpuSkeletalAnimationManager, GPUShaderManager* gpuShaderManager);
    void terminate();
    void begin(const GPURenderPassOutputData& gpuRenderPassOutputData);
    virtual void renderPass(const GPURenderPassOutputData& gpuRenderPassOutputData);
    void end();
    void onResize();
    void addInstanceRendererData(const GPUInstanceRendererData& gpuInstanceRendererData);

protected:
    void compileShader(const GPUInstanceRendererData& gpuInstanceRendererData);
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void preRender();
    virtual void renderGPUInstanceRenderer(const GPUInstanceRendererData& gpuInstanceRendererData);
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual Maths::Matrix4 calculateProjectionViewMatrix() const;

public:
    GPUContext* mGPUContext = nullptr;
protected:
    GPUSkeletalAnimationManager* mGPUSkeletalAnimationManager = nullptr;
    GPUShaderManager* mGPUShaderManager = nullptr;

    GPURenderPassData mGPURenderPassData;

    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    std::unordered_map<GPUInstanceRendererData, Core::OwnerPtr<GPUShaderPipeline>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUShaderPipelines;
    Core::WeakPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    Core::WeakPtr<GPUUniformBuffersContainer> mGlobalGPUUniformBuffersContainer;
    GPUInstanceRendererRegistry mGPUInstanceRendererRegistry;
public:
    CRGET(GPURenderPassData)
};



