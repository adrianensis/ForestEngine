#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPassUI::renderStencilCascade(u64 id)
{    
    FOR_MAP(it, mBatchMap)
	{
        const BatchData& batchData = it->first;
		if(id == batchData.mStencilData.mId)
		{
            if(batchData.mStencilData.mParentId > 0)
            {
                renderStencilCascade(batchData.mStencilData.mParentId);
            }

            if(!mStencilsRendered.contains(batchData.mStencilData.mId))
            {
                mStencilsRendered.insert(batchData.mStencilData.mId);
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

    std::vector<Ptr<BatchRenderer>> noStencilBatches;
    std::vector<Ptr<BatchRenderer>> stencilBatches;
    FOR_MAP(it, mBatchMap)
	{
        if(it->second->getBatchData().mStencilData.mUseStencil)
        {
            if(it->second->getBatchData().mStencilData.mParentId > 0)
            {
                stencilBatches.push_back(it->second);
            }
        }
        else
        {
            noStencilBatches.push_back(it->second);
        }
    }

    auto compareStencilBatch = [](Ptr<BatchRenderer> b1, Ptr<BatchRenderer> b2)
    {
        u64 o1 = b1->getBatchData().mStencilData.mParentId;
        u64 o2 = b2->getBatchData().mStencilData.mParentId;
        return (o1 < o2);
    };
  
    std::sort(stencilBatches.begin(), stencilBatches.end(), compareStencilBatch);

    u64 currentId = 0;
    FOR_LIST(it, stencilBatches)
	{
        const BatchData& batchData = (*it)->getBatchData();
        if(currentId != batchData.mStencilData.mParentId)
        {
            GET_SYSTEM(GPUInterface).clearStencil();
        }

        currentId = batchData.mStencilData.mParentId;

        renderStencilCascade(batchData.mStencilData.mId);
	}

    GET_SYSTEM(GPUInterface).clearStencil();

    FOR_LIST(it, noStencilBatches)
	{
        (*it)->render();
    }
}