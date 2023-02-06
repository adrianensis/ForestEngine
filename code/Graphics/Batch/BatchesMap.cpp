#include "Graphics/Batch/BatchesMap.hpp"
#include "Scene/Module.hpp"

void BatchesMap::addRenderer(Ptr<Renderer> renderer)
{
	BatchData BatchData;
	BatchData.init(renderer);

	if (!MAP_CONTAINS(mBatches, BatchData))
	{
		OwnerPtr<Batch> batch = OwnerPtr<Batch>::newObject();
		batch.get().init(BatchData);

		MAP_INSERT(mBatches, BatchData, batch);
	}

	(mBatches).at(BatchData).get().addRenderer(renderer);
}

void BatchesMap::render()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if( ! it->first.mIsStencilMask)
		{
			it->second.get().render();
		}
	}
}

void BatchesMap::renderStencil()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if(it->first.mIsStencilMask)
		{
			it->second.get().render();
		}
	}
}

void BatchesMap::forceRegenerateBuffers()
{
	FOR_MAP(it, mBatches)
	{
		it->second.get().forceRegenerateBuffers();
	}
}