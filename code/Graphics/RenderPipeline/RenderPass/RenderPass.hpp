#pragma once

#include "Core/Minimal.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"

class RenderPass;

class RenderPassDependency
{
public:
    HashedString mSamplerName;
    GPUFramebufferAttachmentType mAttachmentType;
    WeakPtr<RenderPass> mRenderPass;
};

class RenderPassData
{
public:
    GPURenderPassData mGPURenderPassData;
    GeometricSpace mGeometricSpace = GeometricSpace::WORLD;
    std::vector<RenderPassDependency> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
};

class RenderPass
{
public:
    virtual void init(WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager, const RenderPassData& renderPassData);
    virtual ~RenderPass() = default;
    void terminate();
    virtual void renderPass();
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
    virtual Matrix4 calculateProjectionViewMatrix() const;

protected:
    RenderPassData mRenderPassData;
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    OwnerPtr<GPURenderPass> mGPURenderPass;
    std::unordered_map<GPUInstanceRendererData, OwnerPtr<GPUShaderPipeline>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUShaderPipelines;
    WeakPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    GPUInstanceRendererRegistry mGPUInstanceRendererRegistry;
public:
    RGET(GPUUniformBuffersContainer)
    RGET(GPUInstanceRendererRegistry)
    CRGET(RenderPassData)
    GET(GPURenderPass)
};
REGISTER_CLASS(RenderPass);