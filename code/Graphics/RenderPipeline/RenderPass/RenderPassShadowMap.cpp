#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

void RenderPassShadowMap::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    RenderPass::init(renderPipeline, renderPassData);
    mShaderDepthBuffer = OwnerPtr<ShaderDepthBuffer>::newObject();
}

void RenderPassShadowMap::preFramebufferEnabled()
{
    GET_SYSTEM(GPUInterface).setViewport(0, 0, mRenderPassData.mOutputFramebufferData.mWidth, mRenderPassData.mOutputFramebufferData.mHeight);
}

void RenderPassShadowMap::postFramebufferEnabled()
{
    GET_SYSTEM(GPUInterface).clearDepth();
}

void RenderPassShadowMap::preRender()
{
    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
    GET_SYSTEM(GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
	GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::FRONT);
}

void RenderPassShadowMap::postRender()
{
	GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::NONE);
    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
    
    Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
    GET_SYSTEM(GPUInterface).setViewport(0, 0, windowSize.x, windowSize.y);
}

void RenderPassShadowMap::render()
{
	PROFILER_CPU()
    FOR_MAP(it, mBatches)
	{
        renderBatch(*it);
	}
}

void RenderPassShadowMap::updateGlobalData()
{
	PROFILER_CPU()
    Matrix4 lightProjectionViewMatrix = Matrix4::smIdentity;
    if(mDirectionalLight)
    {
        lightProjectionViewMatrix = mDirectionalLight->getLightProjectionViewMatrix();
    }
    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        lightProjectionViewMatrix,
        camera->mGameObject->mTransform->getWorldPosition()
    };
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<Shader> RenderPassShadowMap::getShader(const BatchData& batchData) const
{
    mShaderDepthBuffer->init(batchData.mMaterial->getShader()->getShaderData());
    return mShaderDepthBuffer;
}