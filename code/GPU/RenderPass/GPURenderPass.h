#pragma once

#include "GPU/Core/GPUContext.hpp"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/Image/GPUImage.h"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"

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
    Core::Ptr<GPUImage> mOutputGPUImage;
    bool mUseDefaultOutput = true;
};

class GPURenderPassOutputData
{
public:
    Core::Ptr<GPUImage> mColorGPUImage;
    Core::Ptr<GPUImage> mDepthGPUImage;
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
    bool init(Core::Ptr<GPUContext> gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager, Core::WeakPtr<GPUUniformBuffersContainer> globalGPUUniformBuffersContainer, const GPURenderPassData& gpuRenderPassData, const GPURenderPassOutputData& gpuRenderPassOutputData);
    void terminate();
    void begin();
    virtual void renderPass();
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

    bool initializeFramebuffers();

public:
    Core::Ptr<GPUContext> mGPUContext;
protected:
    VkRenderPass mRenderPass = VK_NULL_HANDLE;
    std::vector<GPUFramebuffer> framebuffers;

    GPURenderPassData mGPURenderPassData;
    GPURenderPassOutputData mGPURenderPassOutputData;
    GPUFramebuffer mOutputGPUFramebuffer;

    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    std::unordered_map<GPUInstanceRendererData, Core::OwnerPtr<GPUShaderPipeline>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUShaderPipelines;
    Core::WeakPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    Core::WeakPtr<GPUUniformBuffersContainer> mGlobalGPUUniformBuffersContainer;
    GPUInstanceRendererRegistry mGPUInstanceRendererRegistry;
public:
    CRGET(RenderPass)
    CRGET(GPURenderPassData)
    RGET(OutputGPUFramebuffer)
};



