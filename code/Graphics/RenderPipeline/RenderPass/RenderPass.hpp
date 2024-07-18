#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/GPU/GPUFramebuffer.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Core/ECS/ComponentsManager.hpp"

class MeshRenderer;
class RenderPipeline;

class RenderPassData
{
public:
    GeometricSpace mGeometricSpace = GeometricSpace::WORLD;
    std::vector<FramebufferBinding> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
};

class RenderPass: public ObjectBase
{
public:
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
    void terminate();
    void addRenderer(TypedComponentHandler<MeshRenderer> renderer);
    void removeRenderer(TypedComponentHandler<MeshRenderer> renderer);
    virtual void renderPass();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void bindShader(const BatchData& batchData);
    virtual void preRender();
    virtual void renderBatch(const BatchData& batchData);
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual Ptr<Shader> getShader(const BatchData& batchData) const;
    virtual void setupShader(Ptr<Shader> shader) const;
protected:
	std::unordered_set<BatchData, BatchData::BatchDataFunctor> mBatches;
	std::unordered_map<BatchData, OwnerPtr<GPUProgram>, BatchData::BatchDataFunctor> mGPUPrograms;
    RenderPassData mRenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
    Ptr<RenderPipeline> mRenderPipeline;
public:
    CRGET(RenderPassData)
    CRGET(OutputGPUFramebuffer)
};
REGISTER_CLASS(RenderPass);