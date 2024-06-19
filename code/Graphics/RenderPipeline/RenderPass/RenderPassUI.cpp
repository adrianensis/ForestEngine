#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPassUI::preRender()
{
    GET_SYSTEM(GPUInterface).enableFlag(GL_BLEND);
    GET_SYSTEM(GPUInterface).setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderPassUI::postRender()
{
    GET_SYSTEM(GPUInterface).disableFlag(GL_BLEND);
}

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
                it->second.mGPUProgram->enable();
                it->second.mBatch->render();
                it->second.mGPUProgram->disable();
            }

            break;
		}
	}
}

void RenderPassUI::render()
{
	PROFILER_CPU()

    mStencilsRendered.clear();

    std::vector<RenderPassBatchDataWeak> noStencilBatches;
    std::vector<RenderPassBatchDataWeak> stencilBatches;
    FOR_MAP(it, mBatchMap)
	{
        if(it->second.mBatch->getBatchData().mStencilData.mUseStencil)
        {
            if(it->second.mBatch->getBatchData().mStencilData.mParentId > 0)
            {
                stencilBatches.push_back(it->second);
            }
        }
        else
        {
            noStencilBatches.push_back(it->second);
        }
    }

    auto compareStencilBatch = [](RenderPassBatchDataWeak b1, RenderPassBatchDataWeak b2)
    {
        u64 o1 = b1.mBatch->getBatchData().mStencilData.mParentId;
        u64 o2 = b2.mBatch->getBatchData().mStencilData.mParentId;
        return (o1 < o2);
    };
  
    std::sort(stencilBatches.begin(), stencilBatches.end(), compareStencilBatch);

    u64 currentId = 0;
    FOR_LIST(it, stencilBatches)
	{
        const BatchData& batchData = (*it).mBatch->getBatchData();
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
        (*it).mGPUProgram->enable();
        (*it).mBatch->render();
        (*it).mGPUProgram->disable();
    }
}