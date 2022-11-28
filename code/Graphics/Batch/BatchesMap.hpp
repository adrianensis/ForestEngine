#ifndef BATCHESMAP_HPP
#define BATCHESMAP_HPP

#include "Core/Module.hpp"
#include "Graphics/Renderer/Renderer.hpp"
#include "Graphics/Batch/Batch.hpp"

class BatchesMap: public ObjectBase
{
    GENERATE_METADATA(BatchesMap)

public:

    void addRenderer(Ptr<Renderer> renderer);
    void render();
    void renderStencil();
    void forceRegenerateBuffers();

private:
	using BatchMapByBatchData = std::unordered_map<BatchData, OwnerPtr<Batch>, BatchData::BatchDataFunctor>;

	BatchMapByBatchData mBatches;
};

#endif