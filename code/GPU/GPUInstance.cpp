#include "GPU/GPUInstance.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Texture/GPUTextureManager.hpp"

void GPUInstance::init(IGPUWindow* gpuWindow)
{
    mGPUContext = Core::Memory::newObject<GPUContext>();
    mGPUContext->init(gpuWindow);

    mGPUTextureManager = Core::Memory::newObject<GPUTextureManager>();
    mGPUTextureManager->init(mGPUContext);
    mGPUDescriptorManager.init(mGPUContext, mGPUTextureManager);
    mGPUShaderManager = Core::Memory::newObject<GPUShaderManager>();
    mGPUShaderManager->init(mGPUTextureManager);
    mGPUSkeletalAnimationManager = Core::Memory::newObject<GPUSkeletalAnimationManager>();
    mGPUSkeletalAnimationManager->init();
}

void GPUInstance::terminate()
{
    mGPUTextureManager->terminate();
    mGPUShaderManager->terminate();
    mGPUSkeletalAnimationManager->terminate();
    mGPUDescriptorManager.terminate();
    mGPUContext->terminate();
    Core::Memory::deleteObject(mGPUContext);
    Core::Memory::deleteObject(mGPUTextureManager);
    Core::Memory::deleteObject(mGPUShaderManager);
    Core::Memory::deleteObject(mGPUSkeletalAnimationManager);
}