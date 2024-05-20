#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Scene/GameObject.hpp"
#include "Graphics/Material/Shader/ShaderUtils.hpp"
#include "Graphics/Material/MaterialRuntime/MaterialRuntimeShadowMap.hpp"

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

    LightBuiltIn::ShadowMappingData shadowMappingData;
    shadowMappingData.mLightProjectionViewMatrix = projectionViewMatrix;

    GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).setData(shadowMappingData);
}

void RenderPassShadowMap::initShader(Ptr<BatchRenderer> batch)
{
    OwnerPtr<MaterialRuntimeShadowMap> materialRuntime = OwnerPtr<MaterialRuntimeShadowMap>::newObject();
    materialRuntime->initFromCopy(batch->getBatchData().mMaterial->getMaterialRuntime());
    OwnerPtr<GPUProgram> shader = ShaderUtils::createShaderCustomFragment(ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName, batch->getGPUVertexBuffersContainer(), batch->getGPUSharedBuffersContainer(), batch->getBatchData().mMaterial.get(), Ptr<MaterialRuntime>::cast(materialRuntime));
    batch->initShader(shader);
}