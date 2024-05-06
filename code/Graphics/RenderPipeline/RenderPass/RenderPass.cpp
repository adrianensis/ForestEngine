#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

void RenderPass::init(const RenderPassData& renderPassData)
{
    mRenderPassData = renderPassData;
}

void RenderPass::terminate()
{
    FOR_MAP(it, mBatchMap)
	{
        it->second->terminate();
	}
}

void RenderPass::renderBatches()
{
	PROFILER_CPU()
    FOR_MAP(it, mBatchMap)
	{
        it->second->render();
	}
}

void RenderPass::addRenderer(Ptr<MeshRenderer> renderer)
{
	BatchData batchData;
	batchData.init(renderer);

	if (!mBatchMap.contains(batchData))
	{
		mBatchMap.insert_or_assign(batchData, OwnerPtr<BatchRenderer>::newObject());
        mBatchMap.at(batchData)->init(batchData);
	}

	mBatchMap.at(batchData)->addRenderer(renderer);
}

void RenderPass::removeRenderer(Ptr<MeshRenderer> renderer)
{
    BatchData batchData;
	batchData.init(renderer);
    mBatchMap.at(batchData)->removeRenderer(renderer);
}