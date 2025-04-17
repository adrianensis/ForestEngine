#pragma once

#include "Core/Minimal.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"

class RenderPipeline;
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
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
    virtual ~RenderPass() = default;
    void terminate();
    void compileShader(WeakPtr<GPURenderItem> renderItem);
    virtual void renderPass(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass);
    void onResize();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void preRender();
    virtual void renderGPUInstanceRenderer(const GPUInstanceRendererData& gpuInstanceRendererData);
    virtual void render(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass);
    virtual void postRender();
    virtual void updateGlobalData();
    virtual Matrix4 calculateProjectionViewMatrix() const;

protected:
    RenderPassData mRenderPassData;
    Ptr<RenderPipeline> mRenderPipeline;
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    OwnerPtr<GPURenderPass> mGPURenderPass;
    std::unordered_map<GPUInstanceRendererData, OwnerPtr<GPUShaderPipeline>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUShaderPipelines;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(RenderPassData)
    GET(GPURenderPass)
};
REGISTER_CLASS(RenderPass);