#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Window/WindowManager.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderDepthBuffer.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

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
    
    Maths::Vector2 windowSize = GET_SYSTEM(Window::WindowManager).getMainWindow()->getWindowSize();
//    GET_SYSTEM(GPUInterface).setViewport(0, 0, windowSize.x, windowSize.y);
}

void RenderPassShadowMap::render()
{
	PROFILER_CPU()
    FOR_LIST(it, mGPUInstanceRendererRegistry.getGPUInstanceRendererDataSet())
	{
        renderGPUInstanceRenderer(*it);
	}
}

void RenderPassShadowMap::updateGlobalData()
{
	PROFILER_CPU()
    Maths::Matrix4 lightProjectionViewMatrix = Maths::Matrix4::smIdentity;
    if(mDirectionalLight)
    {
        lightProjectionViewMatrix = mDirectionalLight->getLightProjectionViewMatrix();
    }
    EC::TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        lightProjectionViewMatrix,
        ECManager.getFirstComponent<Transform>(camera->getOwnerEntity())->getWorldPosition()
    };
    mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}