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
    void forceRegenerateBuffers();

private:
	using BatchMapByBatchKey = std::unordered_map<BatchKey, OwnerPtr<Batch>, BatchKey::BatchKeyFunctor>;

	BatchMapByBatchKey mBatches;
};

#endif