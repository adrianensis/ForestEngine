#include "GPU/GPUInstance.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"

void GPUInstance::init(IGPUWindow* gpuWindow)
{
//    mMaxUniformBufferBindingPointsUniform = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::UNIFORM);
//    mMaxUniformBufferBindingPointsStorage = GET_SYSTEM(GPUInterface).getMaxBindingPointsForUniformBuffer(GPUBufferType::STORAGE);

    mGPUContext = Core::Memory::newObject<GPUContext>();
    mGPUContext->init(gpuWindow);

    mGPUDescriptorManager.init(mGPUContext);
    mGPUShaderManager = Core::Memory::newObject<GPUShaderManager>();
    mGPUShaderManager->init();
    mGPUSkeletalAnimationManager = Core::Memory::newObject<GPUSkeletalAnimationManager>();
    mGPUSkeletalAnimationManager->init();
}

void GPUInstance::terminate()
{
    mGPUShaderManager->terminate();
    mGPUSkeletalAnimationManager->terminate();
    mGPUDescriptorManager.terminate();
    mGPUContext->terminate();
    Core::Memory::deleteObject(mGPUContext);
    Core::Memory::deleteObject(mGPUShaderManager);
    Core::Memory::deleteObject(mGPUSkeletalAnimationManager);
}