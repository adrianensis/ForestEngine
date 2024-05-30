#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUState.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"

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
        Ptr<BatchRenderer> batch = mBatchMap.at(batchData);
        Ptr<Shader> shader = getShader(batchData);
        setupShader(shader);

        batch->init(ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName, batchData, shader);
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
}

void RenderPass::postRender()
{
	PROFILER_CPU()
}

void RenderPass::render()
{

}

void RenderPass::renderPass()
{
	PROFILER_CPU()

    updateGlobalData();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        preFramebufferEnabled();
        mOutputGPUFramebuffer.enable(GPUFramebufferOperationType::READ_AND_DRAW);
        postFramebufferEnabled();
    }

    preRender();
    render();
    postRender();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        mOutputGPUFramebuffer.disable(GPUFramebufferOperationType::READ_AND_DRAW);
    }
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : Matrix4::smIdentity;

    projectionViewMatrix.mul(viewMatrix);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->mGameObject->mTransform->getWorldPosition()
    };
	GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<Shader> RenderPass::getShader(const BatchData& batchData) const
{
    Ptr<Shader> shader = batchData.mMaterial->getShader();
    return shader;
}

void RenderPass::setupShader(Ptr<Shader> shader) const
{
    FOR_ARRAY(i, mRenderPassData.mDependencies)
    {
        shader->addFramebufferBinding(mRenderPassData.mDependencies[i]);
    }
}