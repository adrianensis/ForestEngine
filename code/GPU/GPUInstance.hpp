#pragma once

#include "Core/Memory/Singleton.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Window/GPUWindow.hpp"
class GPUSkeletalAnimationManager;
class GPUShaderManager;

class GPUInstance: public Core::Singleton<GPUInstance>
{
public:
    virtual void init(IGPUWindow* gpuWindow);
    virtual void terminate();

public:
    GPUContext* mGPUContext = nullptr;
    GPUShaderManager* mGPUShaderManager = nullptr;
    GPUSkeletalAnimationManager* mGPUSkeletalAnimationManager = nullptr;
};
REGISTER_CLASS(GPUInstance)
