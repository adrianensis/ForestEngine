#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"

class MeshRenderer;

class RenderPassData
{
public:
    
};

class RenderPass: public ObjectBase
{
public:
    void init();
    void terminate();
    void onRendererAdded(Ptr<MeshRenderer> renderer);
    void onRendererRemoved(Ptr<MeshRenderer> renderer);
    virtual void render() = 0;
protected:
	using BatchMap = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;
	BatchMap mBatchMap;
};
REGISTER_CLASS(RenderPass);