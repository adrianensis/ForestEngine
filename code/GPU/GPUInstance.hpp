#pragma once

#include "Core/Memory/Singleton.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Window/GPUWindow.hpp"
class GPUSkeletalAnimationManager;
class GPUShaderManager;

class GPUInstance: public Core::Singleton<GPUInstance>
{
public:
    virtual void init(Core::Ptr<IGPUWindow> gpuWindow);
    virtual void terminate();

public:
    Core::OwnerPtr<GPUContext> mGPUContext;
    Core::OwnerPtr<GPUShaderManager> mGPUShaderManager;
    Core::OwnerPtr<GPUSkeletalAnimationManager> mGPUSkeletalAnimationManager;
};
REGISTER_CLASS(GPUInstance)
