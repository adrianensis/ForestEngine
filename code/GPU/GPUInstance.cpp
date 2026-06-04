#include "GPU/GPUInstance.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/Texture/GPUTextureManager.hpp"

void GPUInstance::init(IGPUWindow* gpuWindow)
{
    mGPUContext = new GPUContext();
    mGPUContext->init(gpuWindow);

    mGPUTextureManager = new GPUTextureManager();
    mGPUTextureManager->init(mGPUContext);
    mGPUDescriptorManager.init(mGPUContext, mGPUTextureManager);
    mGPUShaderManager = new GPUShaderManager();
    mGPUShaderManager->init(mGPUTextureManager);
    mGPUSkeletalAnimationManager = new GPUSkeletalAnimationManager();
    mGPUSkeletalAnimationManager->init();
}

void GPUInstance::terminate()
{
    mGPUTextureManager->terminate();
    mGPUShaderManager->terminate();
    mGPUSkeletalAnimationManager->terminate();
    mGPUDescriptorManager.terminate();
    mGPUContext->terminate();
    delete mGPUContext;
    delete mGPUTextureManager;
    delete mGPUShaderManager;
    delete mGPUSkeletalAnimationManager;
}