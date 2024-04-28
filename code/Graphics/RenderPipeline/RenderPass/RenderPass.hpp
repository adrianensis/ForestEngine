#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"

class MeshRenderer;

class RenderPass: public ObjectBase
{
public:
    void init();
    void terminate();
    void addRenderer(Ptr<MeshRenderer> renderer);
    void removeRenderer(Ptr<MeshRenderer> renderer);
    virtual void render() = 0;
protected:
	using BatchMap = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;
	BatchMap mBatchMap;
};
REGISTER_CLASS(RenderPass);