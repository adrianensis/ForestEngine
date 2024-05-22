#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
    
void RenderPassShadowMap::init(const RenderPassData& renderPassData)
{
    RenderPass::init(renderPassData);
    mShader = OwnerPtr<ShaderShadowMap>::newObject();
}

void RenderPassShadowMap::postFramebufferEnabled()
{
    GET_SYSTEM(GPUInterface).clearDepth();
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

    Matrix4 projectionViewMatrix;
    projectionViewMatrix.ortho(-512, 512, -512, 512, 1.0, 1000);
    // projectionViewMatrix.perspective(0.1, 10000, GET_SYSTEM(Window).getAspectRatio(), 90);

    Ptr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    const Matrix4& lightViewMatrix = mPointLight->mGameObject->mTransform->getViewMatrix();
    projectionViewMatrix.mul(lightViewMatrix);

    GPUBuiltIn::SharedBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->mGameObject->mTransform->getWorldPosition()
    };
	GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).setData(gpuGlobalData);
}

Ptr<const Shader> RenderPassShadowMap::getShader(const BatchData& batchData) const
{
    mShader->init(batchData.mMaterial->getShader()->getShaderData());
    return Ptr<const Shader>::cast(mShader);
}