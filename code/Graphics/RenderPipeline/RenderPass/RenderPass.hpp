#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"
#include "Graphics/GPU/GPUFramebuffer.hpp"

class MeshRenderer;

class RenderPassData
{
public:
    std::unordered_set<ClassId> mDependencies;
    GPUFramebufferData mOutputFramebufferData;
};

class RenderPass: public ObjectBase
{
public:
    void init(const RenderPassData& renderPassData);
    void terminate();
    void addRenderer(Ptr<MeshRenderer> renderer);
    void removeRenderer(Ptr<MeshRenderer> renderer);
    virtual void render() = 0;
protected:
	using BatchMap = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;
	BatchMap mBatchMap;
    RenderPassData mRenderPassData;
};
REGISTER_CLASS(RenderPass);