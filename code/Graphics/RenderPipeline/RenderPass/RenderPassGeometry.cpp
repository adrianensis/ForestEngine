#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/Shader/ShaderPBR.hpp"
#include "Graphics/Material/Shader/ShaderManager.hpp"

void RenderPassGeometry::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    RenderPass::init(renderPipeline, renderPassData);
}

void RenderPassGeometry::preRender()
{
    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
    GET_SYSTEM(GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
    GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::BACK);
    GET_SYSTEM(GPUInterface).enableFlag(GPUFlags::BLEND);
    GET_SYSTEM(GPUInterface).setBlendFunc(GPUBlendFactor::SRC_ALPHA, GPUBlendFactor::ONE_MINUS_SRC_ALPHA);
}

void RenderPassGeometry::postRender()
{
    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::BLEND);
    GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::NONE);
    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
    GET_SYSTEM(GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
}

void RenderPassGeometry::render()
{
	PROFILER_CPU()
    FOR_LIST(it, mInstancedMeshRenderers)
	{
        renderBatch(*it);
	}
}

void RenderPassGeometry::updateGlobalData()
{
	PROFILER_CPU()
    RenderPass::updateGlobalData();

    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 lightProjectionViewMatrix = Matrix4::smIdentity;
    if(mDirectionalLight)
    {
        lightProjectionViewMatrix = mDirectionalLight->getLightProjectionViewMatrix();
    }
    LightBuiltIn::ShadowMappingData shadowMappingData;
    shadowMappingData.mLightProjectionViewMatrix = lightProjectionViewMatrix;

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).setData(shadowMappingData);
}

void RenderPassGeometry::bindShader(const InstancedMeshData& instancedMeshData)
{
    RenderPass::bindShader(instancedMeshData);
    mGPUPrograms.at(instancedMeshData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData));
    mGPUPrograms.at(instancedMeshData)->bindSharedBuffer(GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData));
}