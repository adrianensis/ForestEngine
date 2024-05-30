#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/GPU/GPUState.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Window/Window.hpp"

void RenderPassShadowMap::init(const RenderPassData& renderPassData)
{
    RenderPass::init(renderPassData);
    mShaderDepthBuffer = OwnerPtr<ShaderDepthBuffer>::newObject();
}

void RenderPassShadowMap::postFramebufferEnabled()
{
    GET_SYSTEM(GPUInterface).clearDepth();
}

void RenderPassShadowMap::preRender()
{
	GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::FRONT);
}

void RenderPassShadowMap::postRender()
{
	GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::BACK);
}

void RenderPassShadowMap::render()
{
	PROFILER_CPU()
    FOR_MAP(it, mBatchMap)
	{
        it->second->render();
	}
}

void RenderPassShadowMap::updateGlobalData()
{
	PROFILER_CPU()
    const Matrix4 lightProjectionViewMatrix = mDirectionalLight->getLightProjectionViewMatrix();
    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        lightProjectionViewMatrix,
        camera->mGameObject->mTransform->getWorldPosition()
    };
	GET_SYSTEM(GPUState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<Shader> RenderPassShadowMap::getShader(const BatchData& batchData) const
{
    mShaderDepthBuffer->init(batchData.mMaterial->getShader()->getShaderData());
    return mShaderDepthBuffer;
}