#include "Graphics/RenderPipeline/RenderPass/RenderPassBase.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

void RenderPassBase::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 projectionViewMatrix = calculateProjectionViewMatrix();
    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        EC.getFirstComponent<Transform>(camera->getOwnerEntity())->getWorldPosition()
    };
	mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}

Matrix4 RenderPassBase::calculateProjectionViewMatrix() const
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);
    Matrix4 view2D;
    view2D.view(Vector3(0,0,1000), Vector3(0,0,0));

    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mGPURenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->getGPUCamera().mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mGPURenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->getGPUCamera().mViewMatrix : view2D;

    projectionViewMatrix.mul(viewMatrix);

    return projectionViewMatrix;
}