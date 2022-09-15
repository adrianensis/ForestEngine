#include "Graphics/Batch/BatchesMap.hpp"
#include "Scene/Module.hpp"

void BatchesMap::addRenderer(Ptr<Renderer> renderer)
{
	BatchKey batchKey;
	batchKey.init(renderer);

	if (!MAP_CONTAINS(mBatches, batchKey))
	{
		OwnerPtr<Batch> batch = OwnerPtr<Batch>(NEW(Batch));
		batch.get().init(batchKey);

		MAP_INSERT(mBatches, batchKey, batch);
	}

	(mBatches).at(batchKey).get().addRenderer(renderer);
}

void BatchesMap::render()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		it->second.get().render();
	}
}

void BatchesMap::forceRegenerateBuffers()
{
	FOR_MAP(it, mBatches)
	{
		it->second.get().forceRegenerateBuffers();
	}
}