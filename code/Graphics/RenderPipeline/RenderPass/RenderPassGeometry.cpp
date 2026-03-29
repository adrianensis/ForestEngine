#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/Camera/CameraManager.hpp"

void RenderPassGeometry::preRender()
{
//    (GPUInterface).enableFlag(GPUFlags::MULTISAMPLE);
//    (GPUInterface).enableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).setDepthFunc(GPUDepthFunc::LEQUAL);
//    (GPUInterface).setFaceMode(GPUCullFaceType::BACK);
//    (GPUInterface).enableFlag(GPUFlags::BLEND);
//    (GPUInterface).setBlendFunc(GPUBlendFactor::SRC_ALPHA, GPUBlendFactor::ONE_MINUS_SRC_ALPHA);
}

void RenderPassGeometry::postRender()
{
//    (GPUInterface).disableFlag(GPUFlags::BLEND);
//    (GPUInterface).setFaceMode(GPUCullFaceType::NONE);
//    (GPUInterface).disableFlag(GPUFlags::DEPTH_TEST);
//    (GPUInterface).disableFlag(GPUFlags::MULTISAMPLE);
}

void RenderPassGeometry::render()
{
	PROFILER_CPU()
    FOR_LIST(it, mGPUInstanceRendererRegistry.getGPUInstanceRendererDataSet())
	{
        renderGPUInstanceRenderer(*it);
	}
}

void RenderPassGeometry::updateGlobalData()
{
	PROFILER_CPU()
    RenderPassBase::updateGlobalData();

    Camera* camera = mCameraManager->getCamera();

    Maths::Matrix4 lightProjectionViewMatrix = Maths::Matrix4::smIdentity;
    if(mDirectionalLight)
    {
        lightProjectionViewMatrix = mDirectionalLight->getLightProjectionViewMatrix();
    }
    GPULightBuiltIn::ShadowMappingData shadowMappingData;
    shadowMappingData.mLightProjectionViewMatrix = lightProjectionViewMatrix;

    // GPUInstance::getInstance().getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData).setData(shadowMappingData);
}