#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"

class BatchesManager
{
public:

    void terminate();
    void onRendererAdded(Ptr<MeshRenderer> renderer);
    void onRendererRemoved(Ptr<MeshRenderer> renderer);
    void render();
    void renderStencil();
    void renderScreenSpaceStencil();
    void renderScreenSpace();

private:
    void renderScreenSpaceStencilMask(ObjectId maskObjectId);
    void renderStencilMask(ObjectId maskObjectId);

    std::unordered_set<ObjectId> mMasksDrawn;

private:
	using BatchMapByBatchData = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;

	BatchMapByBatchData mBatches;
};
