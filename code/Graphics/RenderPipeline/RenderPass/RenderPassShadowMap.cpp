#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Window/WindowManager.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderDepthBuffer.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"
#include "Scene/GameObject.hpp"

void RenderPassShadowMap::preRender()
{
//    (GPUInterface).clearDepth();
//    (GPUInterface).setViewport(0, 0, mRenderPassData.mOutputFramebufferData.mWidth, mRenderPassData.mOutputFramebufferData.mHeight);

//    (GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    (GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//	(GPUInterface).setFaceMode(GPUCullFaceType::FRONT);
}

void RenderPassShadowMap::postRender()
{
//	(GPUInterface).setFaceMode(GPUCullFaceType::NONE);
//    (GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
    
    // Maths::Vector2 windowSize = (Window::WindowManager).getMainWindow()->getWindowSize();
//    (GPUInterface).setViewport(0, 0, windowSize.x, windowSize.y);
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
    Camera* camera = mCameraManager->getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        lightProjectionViewMatrix,
        GET_SYSTEM(EC::EntityComponentManager).getFirstComponent<Transform>(camera->getOwnerEntity())->getWorldPosition()
    };
    mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}