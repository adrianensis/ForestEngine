#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/Renderer/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/GPU/GPUFramebuffer.hpp"
#include "Graphics/Material/Shader/Shader.hpp"

class MeshRenderer;

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
    virtual void init(const RenderPassData& renderPassData);
    void terminate();
    void addRenderer(Ptr<MeshRenderer> renderer);
    void removeRenderer(Ptr<MeshRenderer> renderer);
    virtual void renderPass();
protected:
    virtual void preFramebufferEnabled();
    virtual void postFramebufferEnabled();
    virtual void preRender();
    virtual void render();
    virtual void postRender();
    virtual void updateGlobalData();
    virtual Ptr<Shader> getShader(const BatchData& batchData) const;
    virtual void setupShader(Ptr<Shader> shader) const;
protected:
	using BatchMap = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;
	BatchMap mBatchMap;
    RenderPassData mRenderPassData;
    GPUFramebuffer mOutputGPUFramebuffer;
public:
    CRGET(RenderPassData)
    CRGET(OutputGPUFramebuffer)
};
REGISTER_CLASS(RenderPass);