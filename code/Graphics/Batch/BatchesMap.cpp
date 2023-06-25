#include "Graphics/Batch/BatchesMap.hpp"
#include "Graphics/RenderContext.hpp"
#include "Scene/Module.hpp"

void BatchesMap::addRenderer(Ptr<MeshRenderer> renderer)
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
		if( ! it->first.mStencilData.mUseStencil and it->first.mIsWorldSpace)
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
		if(it->first.mStencilData.mUseStencil and it->first.mIsWorldSpace)
		{
            ObjectId maskObjectId = it->first.mStencilData.mMaskObjectId;
            if(maskObjectId > 0)
            {
                renderStencilMask(maskObjectId);
            }

			it->second->render();
		}
	}
}

void BatchesMap::renderScreenSpaceStencil()
{
	PROFILER_CPU()

    FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mUseStencil and !it->first.mIsWorldSpace)
		{
            ObjectId maskObjectId = it->first.mStencilData.mMaskObjectId;
            if(maskObjectId > 0)
            {
                renderScreenSpaceStencilMask(maskObjectId);
            }

			it->second->render();
		}
	}
}

void BatchesMap::renderScreenSpaceStencilMask(ObjectId maskObjectId)
{
    FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mUseStencil and maskObjectId == it->first.mStencilData.mThisObjectId and !it->first.mIsWorldSpace)
		{
            if(it->first.mStencilData.mMaskObjectId > 0)
            {
                renderScreenSpaceStencilMask(it->first.mStencilData.mMaskObjectId);
            }
            else
            {
                it->second->setRenderStencilAsMask(true);
                it->second->render();
                it->second->setRenderStencilAsMask(false);
            }
		}
	}
}

void BatchesMap::renderStencilMask(ObjectId maskObjectId)
{
    FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mUseStencil and maskObjectId == it->first.mStencilData.mThisObjectId and it->first.mIsWorldSpace)
		{
            if(it->first.mStencilData.mMaskObjectId > 0)
            {
                renderStencilMask(it->first.mStencilData.mMaskObjectId);
            }
            else
            {
                it->second->setRenderStencilAsMask(true);
                it->second->render();
                it->second->setRenderStencilAsMask(false);
            }
		}
	}
}
void BatchesMap::renderScreenSpace()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if( ! it->first.mStencilData.mUseStencil and !it->first.mIsWorldSpace)
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