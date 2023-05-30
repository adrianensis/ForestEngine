#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/RenderContext.hpp"
#include "Scene/Module.hpp"

void BatchesMap::addRenderer(Ptr<Renderer> renderer)
{
	BatchData BatchData;
	BatchData.init(renderer);

	if (!MAP_CONTAINS(mBatches, BatchData))
	{
		MAP_INSERT(mBatches, BatchData, OwnerPtr<Batch>::newObject());
        (mBatches).at(BatchData)->init(BatchData);
	}

	(mBatches).at(BatchData)->addRenderer(renderer);
}

void BatchesMap::render()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if( ! it->first.mStencilData.mIsStencilMask and it->first.mIsWorldSpace)
		{
			it->second->render();
		}
	}
}

void BatchesMap::renderStencil()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mIsStencilMask)
		{
			it->second->render();
		}
	}
}

void BatchesMap::renderScreenSpace()
{
	PROFILER_CPU()

    GET_SYSTEM(RenderContext).clearDepth();

	FOR_MAP(it, mBatches)
	{
		if(!it->first.mIsWorldSpace)
		{
			it->second->render();
		}
	}
}

void BatchesMap::forceRegenerateBuffers()
{
	FOR_MAP(it, mBatches)
	{
		it->second->forceRegenerateBuffers();
	}
}