#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Core/EntityComponent/EntityPtr.hpp"

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 projectionViewMatrix = calculateProjectionViewMatrix();
    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    GPUShaderDefinitions::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
	mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}

Matrix4 RenderPass::calculateProjectionViewMatrix() const
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);
    Matrix4 view2D;
    view2D.view(Vector3(0,0,1000), Vector3(0,0,0));

    TComponentPtr<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mGPURenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mGPURenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : view2D;

    projectionViewMatrix.mul(viewMatrix);

    return projectionViewMatrix;
}