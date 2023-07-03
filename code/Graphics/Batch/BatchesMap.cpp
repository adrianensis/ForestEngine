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

    // FOR_MAP(it, mBatches)
	// {
	// 	if(it->first.mStencilData.mUseStencil and it->first.mIsWorldSpace)
	// 	{
    //         ObjectId maskObjectId = it->first.mStencilData.mMaskObjectId;
    //         if(maskObjectId > 0)
    //         {
    //             renderStencilMask(maskObjectId);
    //         }

	// 		it->second->render();
	// 	}
	// }
}

void BatchesMap::renderStencilMask(ObjectId maskObjectId)
{
    // FOR_MAP(it, mBatches)
	// {
	// 	if(it->first.mStencilData.mUseStencil and maskObjectId == it->first.mStencilData.mThisObjectId and it->first.mIsWorldSpace)
	// 	{
    //         if(it->first.mStencilData.mMaskObjectId > 0)
    //         {
    //             renderStencilMask(it->first.mStencilData.mMaskObjectId);

    //             it->second->render();
    //         }
    //         else
    //         {
    //             it->second->render();
    //         }

    //         break;
	// 	}
	// }
}

void BatchesMap::renderScreenSpaceStencil()
{
	PROFILER_CPU()

    mMasksDrawn.clear();

    std::vector<Ptr<Batch>> sortedBatches;
    FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mUseStencil and !it->first.mIsWorldSpace)
		{
            sortedBatches.push_back(it->second);
        }
    }

    auto compareStencilBatch = [](Ptr<Batch> b1, Ptr<Batch> b2)
    {
        ObjectId o1 = b1->getBatchData().mStencilData.mMaskObjectId > 0 ? b1->getBatchData().mStencilData.mMaskObjectId : b1->getBatchData().mStencilData.mThisObjectId;
        ObjectId o2 = b2->getBatchData().mStencilData.mMaskObjectId > 0 ? b2->getBatchData().mStencilData.mMaskObjectId : b2->getBatchData().mStencilData.mThisObjectId;
        return (o1 < o2);
    };
  
    std::sort(sortedBatches.begin(), sortedBatches.end(), compareStencilBatch);

    ObjectId currentMaskId = 0;
    FOR_LIST(it, sortedBatches)
	{
        ObjectId maskId = (*it)->getBatchData().mStencilData.mMaskObjectId > 0 ? (*it)->getBatchData().mStencilData.mMaskObjectId : (*it)->getBatchData().mStencilData.mThisObjectId;
        if(currentMaskId != maskId)
        {
            currentMaskId = maskId;
            GET_SYSTEM(RenderContext).clearStencil();
        }

		if((*it)->getBatchData().mStencilData.mUseStencil and !(*it)->getBatchData().mIsWorldSpace)
		{
            if(!mMasksDrawn.contains((*it)->getBatchData().mStencilData.mThisObjectId) || (*it)->getBatchData().mStencilData.mThisObjectId == 0)
            {
                mMasksDrawn.insert((*it)->getBatchData().mStencilData.mThisObjectId);
                
                ObjectId maskObjectId = (*it)->getBatchData().mStencilData.mMaskObjectId;
                if(maskObjectId > 0)
                {
                    renderScreenSpaceStencilMask(maskObjectId);
                }
                
			    (*it)->render();
            }
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

            if(!mMasksDrawn.contains(it->first.mStencilData.mThisObjectId))
            {
                mMasksDrawn.insert(it->first.mStencilData.mThisObjectId);
                it->second->render();
            }

            break;
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

void BatchesMap::requestRegenerateBuffers()
{
	FOR_MAP(it, mBatches)
	{
		it->second->requestRegenerateBuffers();
	}
}