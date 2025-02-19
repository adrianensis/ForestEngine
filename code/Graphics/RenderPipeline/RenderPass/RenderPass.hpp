#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"

class MeshRenderer;
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
    void addRenderer(TComponentHandler<MeshRenderer> renderer);
    OwnerPtr<GPUShaderPipeline> compileShader(TComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TComponentHandler<MeshRenderer> renderer);
    virtual void renderPass();
    void compile();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void preRender();
    virtual void renderInstancedMesh(const InstancedMeshData& instancedMeshData);
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual Matrix4 calculateProjectionViewMatrix() const;

protected:
	std::unordered_set<InstancedMeshData, InstancedMeshData::InstancedMeshDataFunctor> mInstancedMeshRenderers;
    RenderPassData mRenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
    Ptr<RenderPipeline> mRenderPipeline;
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    GPURenderPass* mGPURenderPass = nullptr;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(RenderPassData)
    CRGET(OutputGPUFramebuffer)
    GET(GPURenderPass)
};
REGISTER_CLASS(RenderPass);