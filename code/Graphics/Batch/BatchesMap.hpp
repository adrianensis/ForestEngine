#pragma once

#include "Core/Module.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Batch/Batch.hpp"

class BatchesMap: public ObjectBase
{
    GENERATE_METADATA(BatchesMap)

public:

    void addRenderer(Ptr<MeshRenderer> renderer);
    void render();
    void renderStencil();
    void renderScreenSpaceStencil();
    void renderScreenSpace();
    void forceRegenerateBuffers();

private:
    void renderScreenSpaceStencilMask(ObjectId maskObjectId);
    void renderStencilMask(ObjectId maskObjectId);

private:
	using BatchMapByBatchData = std::unordered_map<BatchData, OwnerPtr<Batch>, BatchData::BatchDataFunctor>;

	BatchMapByBatchData mBatches;
};
