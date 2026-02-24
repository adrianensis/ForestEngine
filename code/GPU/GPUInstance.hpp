#pragma once

#include "Core/Memory/Singleton.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Window/GPUWindow.hpp"
#include "GPU/Descriptors/GPUDescriptorManager.hpp"
class GPUSkeletalAnimationManager;
class GPUShaderManager;
class GPUTextureManager;

class GPUInstance: public Core::Singleton<GPUInstance>
{
public:
    virtual void init(IGPUWindow* gpuWindow);
    virtual void terminate();

public:
    GPUContext* mGPUContext = nullptr;
    GPUTextureManager* mGPUTextureManager = nullptr;
    GPUShaderManager* mGPUShaderManager = nullptr;
    GPUSkeletalAnimationManager* mGPUSkeletalAnimationManager = nullptr;
    GPUDescriptorManager mGPUDescriptorManager;
};