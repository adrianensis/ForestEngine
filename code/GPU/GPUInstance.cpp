#include "GPU/GPUInstance.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"

void GPUInstance::init(Core::Ptr<IGPUWindow> gpuWindow)
{
//    mMaxUniformBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::UNIFORM);
//    mMaxUniformBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::STORAGE);

    mGPUContext = Core::OwnerPtr<GPUContext>::newObject();
    mGPUContext->init(gpuWindow);

    mGPUShaderManager = Core::OwnerPtr<GPUShaderManager>::newObject();
    mGPUShaderManager->init();
    mGPUSkeletalAnimationManager = Core::OwnerPtr<GPUSkeletalAnimationManager>::newObject();
    mGPUSkeletalAnimationManager->init();
}

void GPUInstance::terminate()
{
    mGPUShaderManager->terminate();
    mGPUSkeletalAnimationManager->terminate();
    mGPUContext->terminate();
}