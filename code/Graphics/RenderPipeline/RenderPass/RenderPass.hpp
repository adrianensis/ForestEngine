#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "Core/ECS/ComponentsManager.hpp"

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
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class RenderPassData
{
public:
    GeometricSpace mGeometricSpace = GeometricSpace::WORLD;
    std::vector<RenderPassDependency> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
};

class RenderPass
{
public:
    virtual void init(WeakPtr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
    void terminate();
    void addRenderer(TComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TComponentHandler<MeshRenderer> renderer);
    virtual void renderPass();
    void compile();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void bindShader(const InstancedMeshData& instancedMeshData);
    virtual void preRender();
    virtual void renderInstancedMesh(const InstancedMeshData& instancedMeshData);
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual void setupShader(WeakPtr<Shader> shader) const;

protected:
	std::unordered_set<InstancedMeshData, InstancedMeshData::InstancedMeshDataFunctor> mInstancedMeshRenderers;
	std::unordered_map<InstancedMeshData, OwnerPtr<GPUShader>, InstancedMeshData::InstancedMeshDataFunctor> mGPUShaders;
    RenderPassData mRenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
    WeakPtr<RenderPipeline> mRenderPipeline;

public:
    CRGET(RenderPassData)
    CRGET(OutputGPUFramebuffer)
};
REGISTER_CLASS(RenderPass);