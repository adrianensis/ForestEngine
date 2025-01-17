#pragma once

#include "Core/System/System.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Core/GPUContext.hpp"

class GPUInstance: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    u32 requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType);

private:
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxUniformBufferBindingPointsUniform = 0;
    i32 mMaxUniformBufferBindingPointsStorage = 0;

public:
    OwnerPtr<GPUContext> mGPUContext;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(GPUUniformBuffersContainer)
};
REGISTER_CLASS(GPUInstance)
