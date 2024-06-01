#pragma once

#include "Engine/System/System.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class GPUGlobalState: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    u32 requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType);
private:
    GPUSharedBuffersContainer mGPUSharedBuffersContainer;
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxSharedBufferBindingPointsUniform = 0;
    i32 mMaxSharedBufferBindingPointsStorage = 0;

public:
    RGET(GPUSharedBuffersContainer)
    CRGET(GPUSharedBuffersContainer)
};
REGISTER_CLASS(GPUGlobalState)
