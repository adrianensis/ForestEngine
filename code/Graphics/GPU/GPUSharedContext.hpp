#pragma once

#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"

class GPUSharedContext : public System
{
public:
	GPUSharedContext() = default;

    void init();
    void terminate();
    u32 requestSharedBufferBindingPoint(GPUBufferType gpuSharedBufferType);
    
private:
    GPUSharedBuffersContainer mGPUSharedBuffersContainer;

private:
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxSharedBufferBindingPointsUniform = 0;
    i32 mMaxSharedBufferBindingPointsStorage = 0;

public:
    RGET(GPUSharedBuffersContainer)
    CRGET(GPUSharedBuffersContainer)
};