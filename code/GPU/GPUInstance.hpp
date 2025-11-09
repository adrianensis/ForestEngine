#pragma once

#include "Core/Memory/Singleton.hpp"
#include "GPU/Core/GPUContext.hpp"
#include "GPU/Mesh/GPUMeshFactory.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "GPU/Window/GPUWindow.hpp"

class GPUInstance: public Core::Singleton<GPUInstance>
{
public:
    virtual void init(Core::Ptr<IGPUWindow> gpuWindow);
    virtual void terminate();

public:
    Core::OwnerPtr<GPUContext> mGPUContext;
    Core::OwnerPtr<GPUMeshFactory> mGPUMeshFactory;
    Core::OwnerPtr<GPUShaderManager> mGPUShaderManager;
    Core::OwnerPtr<GPUSkeletalAnimationManager> mGPUSkeletalAnimationManager;
};
REGISTER_CLASS(GPUInstance)
