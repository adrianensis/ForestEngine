#pragma once

#include "Core/System/System.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/Core/GPUContext.hpp"

class GPUInstance: public Core::System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    Core::u32 requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType);

private:
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    Core::u32 mBindingPointsIndexUniform = 0;
    Core::u32 mBindingPointsIndexStorage = 0;
    Core::i32 mMaxUniformBufferBindingPointsUniform = 0;
    Core::i32 mMaxUniformBufferBindingPointsStorage = 0;

public:
    Core::OwnerPtr<GPUContext> mGPUContext;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(GPUUniformBuffersContainer)
};
REGISTER_CLASS(GPUInstance)
