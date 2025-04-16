#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Core/Window/WindowManager.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderDepthBuffer.hpp"
#include "Core/EntityComponent/EntityPtr.hpp"

void RenderPassShadowMap::preRender()
{
//    GET_SYSTEM(GPUInterface).clearDepth();
//    GET_SYSTEM(GPUInterface).setViewport(0, 0, mRenderPassData.mOutputFramebufferData.mWidth, mRenderPassData.mOutputFramebufferData.mHeight);

//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    GET_SYSTEM(GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//	GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::FRONT);
}

void RenderPassShadowMap::postRender()
{
//	GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::NONE);
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
    
    Vector2 windowSize = GET_SYSTEM(WindowManager).getMainWindow()->getWindowSize();
//    GET_SYSTEM(GPUInterface).setViewport(0, 0, windowSize.x, windowSize.y);
}

void RenderPassShadowMap::render(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass)
{
	PROFILER_CPU()
    FOR_MAP(it, gpuInstanceRendererDataByRenderPass)
	{
        renderGPUInstanceRenderer(*it);
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
    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        lightProjectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
    mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}