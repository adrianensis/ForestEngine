#include "GPU/GPUInstance.hpp"

void GPUInstance::init(Core::Ptr<IGPUWindow> gpuWindow)
{
//    mMaxUniformBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::UNIFORM);
//    mMaxUniformBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::STORAGE);

    mGPUContext = Core::OwnerPtr<GPUContext>::newObject();
    mGPUContext->init(gpuWindow);

    mGPUMeshFactory = Core::OwnerPtr<GPUMeshFactory>::newObject();
    mGPUMeshFactory->init();
    mGPUShaderManager = Core::OwnerPtr<GPUShaderManager>::newObject();
    mGPUShaderManager->init();
    mGPUSkeletalAnimationManager = Core::OwnerPtr<GPUSkeletalAnimationManager>::newObject();
    mGPUSkeletalAnimationManager->init();
}

void GPUInstance::terminate()
{
    mGPUMeshFactory->terminate();
    mGPUShaderManager->terminate();
    mGPUSkeletalAnimationManager->terminate();
    mGPUContext->terminate();
}