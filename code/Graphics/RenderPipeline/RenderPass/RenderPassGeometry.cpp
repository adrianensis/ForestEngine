#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"

void RenderPassGeometry::preRender()
{
    GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::BACK);
    GET_SYSTEM(GPUInterface).enableFlag(GL_BLEND);
    GET_SYSTEM(GPUInterface).setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderPassGeometry::postRender()
{
    GET_SYSTEM(GPUInterface).disableFlag(GL_BLEND);
    GET_SYSTEM(GPUInterface).setFaceMode(GPUCullFaceType::NONE);
}

void RenderPassGeometry::render()
{
	PROFILER_CPU()
    FOR_LIST(it, mBatches)
	{
        Ptr<BatchRenderer> batchRenderer = mRenderPipeline->getBatchMap().at(ClassManager::getDynamicClassMetadata(this).mClassDefinition.getId()).at(*it);
        mGPUPrograms.at(*it)->enable();
        batchRenderer->render();
        mGPUPrograms.at(*it)->disable();
	}
}

void RenderPassGeometry::updateGlobalData()
{
	PROFILER_CPU()
    RenderPass::updateGlobalData();

    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 lightProjectionViewMatrix = Matrix4::smIdentity;
    if(mDirectionalLight)
    {
        lightProjectionViewMatrix = mDirectionalLight->getLightProjectionViewMatrix();
    }
    LightBuiltIn::ShadowMappingData shadowMappingData;
    shadowMappingData.mLightProjectionViewMatrix = lightProjectionViewMatrix;

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).setData(shadowMappingData);
}