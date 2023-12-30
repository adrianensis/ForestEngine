#include "Graphics/BatchRenderer/BatchesManager.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Scene/Module.hpp"

void BatchesManager::terminate()
{
    FOR_MAP(it, mBatches)
	{
        it->second->terminate();
	}
}

void BatchesManager::addRenderer(Ptr<MeshRenderer> renderer)
{
	BatchData BatchData;
	BatchData.init(renderer);

	if (!mBatches.contains(BatchData))
	{
		mBatches.insert_or_assign(BatchData, OwnerPtr<BatchRenderer>::newObject());
        (mBatches).at(BatchData)->init(BatchData);
	}

	(mBatches).at(BatchData)->addRenderer(renderer);
}

void BatchesManager::render()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if( ! it->first.mStencilData.mUseStencil && it->first.mIsWorldSpace)
		{
			it->second->render();
		}
	}
}

void BatchesManager::renderStencil()
{
	PROFILER_CPU()

    // FOR_MAP(it, mBatches)
	// {
	// 	if(it->first.mStencilData.mUseStencil && it->first.mIsWorldSpace)
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

void BatchesManager::renderStencilMask(ObjectId maskObjectId)
{
    // FOR_MAP(it, mBatches)
	// {
	// 	if(it->first.mStencilData.mUseStencil && maskObjectId == it->first.mStencilData.mThisObjectId && it->first.mIsWorldSpace)
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

void BatchesManager::renderScreenSpaceStencil()
{
	PROFILER_CPU()

    mMasksDrawn.clear();

    std::vector<Ptr<BatchRenderer>> sortedBatches;
    FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mUseStencil && !it->first.mIsWorldSpace)
		{
            sortedBatches.push_back(it->second);
        }
    }

    auto compareStencilBatch = [](Ptr<BatchRenderer> b1, Ptr<BatchRenderer> b2)
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
            GET_SYSTEM(GPUInterface).clearStencil();
        }

		if((*it)->getBatchData().mStencilData.mUseStencil && !(*it)->getBatchData().mIsWorldSpace)
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

void BatchesManager::renderScreenSpaceStencilMask(ObjectId maskObjectId)
{    
    FOR_MAP(it, mBatches)
	{
		if(it->first.mStencilData.mUseStencil && maskObjectId == it->first.mStencilData.mThisObjectId && !it->first.mIsWorldSpace)
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

void BatchesManager::renderScreenSpace()
{
	PROFILER_CPU()

	FOR_MAP(it, mBatches)
	{
		if( ! it->first.mStencilData.mUseStencil && !it->first.mIsWorldSpace)
		{
			it->second->render();
		}
	}
}

void BatchesManager::requestRegenerateBuffers()
{
	FOR_MAP(it, mBatches)
	{
		it->second->requestRegenerateBuffers();
	}
}