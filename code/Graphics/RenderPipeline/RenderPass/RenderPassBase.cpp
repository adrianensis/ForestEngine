#include "Graphics/RenderPipeline/RenderPass/RenderPassBase.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

void RenderPassBase::updateGlobalData()
{
	PROFILER_CPU()

    Maths::Matrix4 projectionViewMatrix = calculateProjectionViewMatrix();
    EC::ComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        ECManager.getFirstComponent<Transform>(camera->getOwnerEntity())->getWorldPosition()
    };
	mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}

Maths::Matrix4 RenderPassBase::calculateProjectionViewMatrix() const
{
	PROFILER_CPU()

    Maths::Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);
    Maths::Matrix4 view2D;
    view2D.view(Maths::Vector3(0,0,1000), Maths::Vector3(0,0,0));

    EC::ComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Maths::Matrix4 projectionViewMatrix = mGPURenderPassData.mGeometricSpace == Maths::GeometricSpace::WORLD ? camera->getGPUCamera().mProjectionMatrix : ortho;
    Maths::Matrix4 viewMatrix = mGPURenderPassData.mGeometricSpace == Maths::GeometricSpace::WORLD ? camera->getGPUCamera().mViewMatrix : view2D;

    projectionViewMatrix.mul(viewMatrix);

    return projectionViewMatrix;
}