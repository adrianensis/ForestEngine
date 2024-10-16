#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/Shader/ShaderDepthBuffer.hpp"
#include "Graphics/Material/Shader/ShaderManager.hpp"
#include "Core/ECS/EntityHandler.hpp"

void RenderPassShadowMap::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    RenderPass::init(renderPipeline, renderPassData);
    mShaderShadowMap = GET_SYSTEM(ShaderManager).createShader<ShaderDepthBuffer>();
}

void RenderPassShadowMap::preRender()
{
    GET_SYSTEM(GPUInterface).clearDepth();
    GET_SYSTEM(GPUInterface).setViewport(0, 0, mRenderPassData.mOutputFramebufferData.mWidth, mRenderPassData.mOutputFramebufferData.mHeight);

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
    FOR_MAP(it, mInstancedMeshRenderers)
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
    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        lightProjectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<Shader> RenderPassShadowMap::getShader(const InstancedMeshData& instancedMeshData) const
{
    return mShaderShadowMap;
}
