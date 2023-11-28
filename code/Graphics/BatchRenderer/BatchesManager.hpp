#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/BatchRenderer/BatchRenderer.hpp"

class BatchesManager: public ObjectBase
{
    GENERATE_METADATA(BatchesManager)

public:

    void addRenderer(Ptr<MeshRenderer> renderer);
    void render();
    void renderStencil();
    void renderScreenSpaceStencil();
    void renderScreenSpace();
    void requestRegenerateBuffers();

private:
    void renderScreenSpaceStencilMask(ObjectId maskObjectId);
    void renderStencilMask(ObjectId maskObjectId);

    std::set<ObjectId> mMasksDrawn;

private:
	using BatchMapByBatchData = std::unordered_map<BatchData, OwnerPtr<BatchRenderer>, BatchData::BatchDataFunctor>;

	BatchMapByBatchData mBatches;
};
