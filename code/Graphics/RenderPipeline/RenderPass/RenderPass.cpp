#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"

void RenderPass::init()
{

}

void RenderPass::terminate()
{
    FOR_MAP(it, mBatchMap)
	{
        it->second->terminate();
	}
}

void RenderPass::onRendererAdded(Ptr<MeshRenderer> renderer)
{
	BatchData batchData;
	batchData.init(renderer);

	if (!mBatchMap.contains(batchData))
	{
		mBatchMap.insert_or_assign(batchData, OwnerPtr<BatchRenderer>::newObject());
        mBatchMap.at(batchData)->init(batchData);
	}

	mBatchMap.at(batchData)->onAddRenderer(renderer);
}

void RenderPass::onRendererRemoved(Ptr<MeshRenderer> renderer)
{
    BatchData batchData;
	batchData.init(renderer);
    mBatchMap.at(batchData)->onRemoveRenderer(renderer);
}