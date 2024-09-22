#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Renderer/InstancedMeshRenderer/InstancedMeshRenderer.hpp"
#include "Graphics/GPU/Framebuffer/GPUFramebuffer.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Core/ECS/ComponentsManager.hpp"

#include "Graphics/GPU/Framebuffer/GPUFramebuffer.hpp"
#include "Graphics/GPU/RenderPass/GPURenderPass.h"

class MeshRenderer;
class RenderPipeline;
class RenderPass;

class RenderPassDependency
{
public:
    HashedString mSamplerName;
    GPUFramebufferAttachmentType mAttachmentType;
    Ptr<RenderPass> mRenderPass;
    GPUPipelineStage mStage = GPUPipelineStage::NONE;
};

class RenderPassData
{
public:
    GeometricSpace mGeometricSpace = GeometricSpace::WORLD;
    std::vector<RenderPassDependency> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
    Ptr<Shader> mShader;
};

class RenderPass: public ObjectBase
{
public:
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
    void terminate();
    void addRenderer(TypedComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TypedComponentHandler<MeshRenderer> renderer);
    virtual void renderPass();
    void compile();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void bindShader(const InstancedMeshData& instancedMeshData);
    virtual void preRender();
    virtual void renderBatch(const InstancedMeshData& instancedMeshData);
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual void setupShader(Ptr<Shader> shader) const;

protected:
	std::unordered_set<InstancedMeshData, InstancedMeshData::InstancedMeshDataFunctor> mInstancedMeshRenderers;
	std::unordered_map<InstancedMeshData, OwnerPtr<GPUShader>, InstancedMeshData::InstancedMeshDataFunctor> mGPUShaders;
    RenderPassData mRenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
    Ptr<RenderPipeline> mRenderPipeline;

    GPURenderPass* vulkanRenderPass;

public:
    CRGET(RenderPassData)
    CRGET(OutputGPUFramebuffer)
};
REGISTER_CLASS(RenderPass);