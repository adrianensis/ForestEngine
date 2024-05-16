#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"

void RenderPass::init(const RenderPassData& renderPassData)
{
    mRenderPassData = renderPassData;

    if(mRenderPassData.mOutputFramebufferData.isValid())
    { 
        mOutputGPUFramebuffer.init(mRenderPassData.mOutputFramebufferData);
    }
}

void RenderPass::terminate()
{
    FOR_MAP(it, mBatchMap)
	{
        it->second->terminate();
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

void RenderPass::preFramebufferEnabled()
{

}

void RenderPass::postFramebufferEnabled()
{

}

void RenderPass::preRender()
{
	PROFILER_CPU()

    updateGlobalData();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        preFramebufferEnabled();
        mOutputGPUFramebuffer.enable(GPUFramebufferOperationType::READ_AND_DRAW);
        postFramebufferEnabled();
    }
}

void RenderPass::postRender()
{
	PROFILER_CPU()
    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        mOutputGPUFramebuffer.disable(GPUFramebufferOperationType::READ_AND_DRAW);
    }
}

void RenderPass::render()
{

}

void RenderPass::renderPass()
{
	PROFILER_CPU()
    preRender();
    render();
    postRender();
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho,
        mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : Matrix4::smIdentity,
        camera->mGameObject->mTransform->getWorldPosition()
    };
	GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}