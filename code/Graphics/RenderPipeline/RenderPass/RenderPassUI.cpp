#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

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
    FOR_LIST(it, mBatches)
	{
        const BatchData& batchData = *it;
		if(id == batchData.mStencilData.mId)
		{
            if(batchData.mStencilData.mParentId > 0)
            {
                renderStencilCascade(batchData.mStencilData.mParentId);
            }

            if(!mStencilsRendered.contains(batchData.mStencilData.mId))
            {
                mStencilsRendered.insert(batchData.mStencilData.mId);
                renderBatch(batchData);
            }

            break;
		}
	}
}

void RenderPassUI::render()
{
	PROFILER_CPU()

    mStencilsRendered.clear();

    std::vector<BatchData> noStencilBatches;
    std::vector<BatchData> stencilBatches;
    FOR_LIST(it, mBatches)
	{
        const BatchData& batchData = *it;
        Ptr<BatchRenderer> batchRenderer = mRenderPipeline->getBatchMap().at(batchData);
        if(batchData.mStencilData.mUseStencil)
        {
            if(batchData.mStencilData.mParentId > 0)
            {
                stencilBatches.push_back(batchData);
            }
        }
        else
        {
            noStencilBatches.push_back(batchData);
        }
    }

    auto compareStencilBatch = [](BatchData b1, BatchData b2)
    {
        u64 o1 = b1.mStencilData.mParentId;
        u64 o2 = b2.mStencilData.mParentId;
        return (o1 < o2);
    };
  
    std::sort(stencilBatches.begin(), stencilBatches.end(), compareStencilBatch);

    u64 currentId = 0;
    FOR_LIST(it, stencilBatches)
	{
        const BatchData& batchData = *it;
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
        renderBatch(*it);
    }
}