#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPassUI::render()
{
	PROFILER_CPU()
	FOR_MAP(it, mBatchMap)
	{
		it->second->render();
	}
}

void RenderPassUIStencil::renderStencilMask(ObjectId maskObjectId)
{    
    FOR_MAP(it, mBatchMap)
	{
		if(it->first.mStencilData.mUseStencil && maskObjectId == it->first.mStencilData.mId && !it->first.mIsWorldSpace)
		{
            if(it->first.mStencilData.mParentId > 0)
            {
                renderStencilMask(it->first.mStencilData.mParentId);
            }

            if(!mMasksDrawn.contains(it->first.mStencilData.mId))
            {
                mMasksDrawn.insert(it->first.mStencilData.mId);
                it->second->render();
            }

            break;
		}
	}
}

void RenderPassUIStencil::render()
{
	PROFILER_CPU()

    mMasksDrawn.clear();

    std::vector<Ptr<BatchRenderer>> sortedBatches;
    FOR_MAP(it, mBatchMap)
	{
		if(it->first.mStencilData.mUseStencil && !it->first.mIsWorldSpace)
		{
            sortedBatches.push_back(it->second);
        }
    }

    auto compareStencilBatch = [](Ptr<BatchRenderer> b1, Ptr<BatchRenderer> b2)
    {
        u64 o1 = b1->getBatchData().mStencilData.mParentId > 0 ? b1->getBatchData().mStencilData.mParentId : b1->getBatchData().mStencilData.mId;
        u64 o2 = b2->getBatchData().mStencilData.mParentId > 0 ? b2->getBatchData().mStencilData.mParentId : b2->getBatchData().mStencilData.mId;
        return (o1 < o2);
    };
  
    std::sort(sortedBatches.begin(), sortedBatches.end(), compareStencilBatch);

    u64 currentMaskId = 0;
    FOR_LIST(it, sortedBatches)
	{
        u64 maskId = (*it)->getBatchData().mStencilData.mParentId > 0 ? (*it)->getBatchData().mStencilData.mParentId : (*it)->getBatchData().mStencilData.mId;
        if(currentMaskId != maskId)
        {
            currentMaskId = maskId;
            GET_SYSTEM(GPUInterface).clearStencil();
        }

		if((*it)->getBatchData().mStencilData.mUseStencil && !(*it)->getBatchData().mIsWorldSpace)
		{
            if(!mMasksDrawn.contains((*it)->getBatchData().mStencilData.mId) || (*it)->getBatchData().mStencilData.mId == 0)
            {
                mMasksDrawn.insert((*it)->getBatchData().mStencilData.mId);
                
                u64 maskId = (*it)->getBatchData().mStencilData.mParentId;
                if(maskId > 0)
                {
                    renderStencilMask(maskId);
                }
                
			    (*it)->render();
            }
		}
	}
}