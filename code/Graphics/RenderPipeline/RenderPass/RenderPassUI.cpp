#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPassUI::renderStencil(u64 id)
{    
    FOR_MAP(it, mBatchMap)
	{
		if(id == it->first.mStencilData.mId)
		{
            if(it->first.mStencilData.mParentId > 0)
            {
                renderStencil(it->first.mStencilData.mParentId);
            }

            if(!mStencilsRendered.contains(it->first.mStencilData.mId))
            {
                mStencilsRendered.insert(it->first.mStencilData.mId);
                it->second->render();
            }

            break;
		}
	}
}

void RenderPassUI::render()
{
	PROFILER_CPU()

    mStencilsRendered.clear();

    std::vector<Ptr<BatchRenderer>> sortedBatches;
    FOR_MAP(it, mBatchMap)
	{
        sortedBatches.push_back(it->second);
    }

    auto compareStencilBatch = [](Ptr<BatchRenderer> b1, Ptr<BatchRenderer> b2)
    {
        u64 o1 = b1->getBatchData().mStencilData.mParentId > 0 ? b1->getBatchData().mStencilData.mParentId : b1->getBatchData().mStencilData.mId;
        u64 o2 = b2->getBatchData().mStencilData.mParentId > 0 ? b2->getBatchData().mStencilData.mParentId : b2->getBatchData().mStencilData.mId;
        return (o1 < o2);
    };
  
    std::sort(sortedBatches.begin(), sortedBatches.end(), compareStencilBatch);

    u64 currentId = 0;
    FOR_LIST(it, sortedBatches)
	{
        u64 id = (*it)->getBatchData().mStencilData.mParentId > 0 ? (*it)->getBatchData().mStencilData.mParentId : (*it)->getBatchData().mStencilData.mId;
        if(currentId != id)
        {
            currentId = id;
            GET_SYSTEM(GPUInterface).clearStencil();
        }

        if(!mStencilsRendered.contains((*it)->getBatchData().mStencilData.mId) || (*it)->getBatchData().mStencilData.mId == 0)
        {
            mStencilsRendered.insert((*it)->getBatchData().mStencilData.mId);
            
            u64 parentId = (*it)->getBatchData().mStencilData.mParentId;
            if(parentId > 0)
            {
                renderStencil(parentId);
            }
            
            (*it)->render();
        }
	}
}