#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Window/Window.hpp"

void RenderPassGeometry::preRender()
{
    GET_SYSTEM(GPUInterface).enableFlag(GL_BLEND);
    GET_SYSTEM(GPUInterface).setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderPassGeometry::postRender()
{
    GET_SYSTEM(GPUInterface).disableFlag(GL_BLEND);
}

void RenderPassGeometry::render()
{
	PROFILER_CPU()
    FOR_MAP(it, mBatchMap)
	{
        it->second->render();
	}
}

void RenderPassGeometry::updateGlobalData()
{
	PROFILER_CPU()
    RenderPass::updateGlobalData();

    Matrix4 projectionViewMatrix;
    // projectionViewMatrix.ortho(-512, 512, -512, 512, 1.0, 1000);
    projectionViewMatrix.perspective(0.1, 10000, GET_SYSTEM(Window).getAspectRatio(), 90);

    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    const Matrix4& lightViewMatrix = mDirectionalLight->mGameObject->mTransform->getViewMatrix();
    projectionViewMatrix.mul(lightViewMatrix);

    LightBuiltIn::ShadowMappingData shadowMappingData;
    shadowMappingData.mLightProjectionViewMatrix = projectionViewMatrix;

    GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).setData(shadowMappingData);
}