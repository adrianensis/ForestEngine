#pragma once

#include "Core/ECS/System.hpp"
#include "Graphics/GPU/Buffer/GPUBuffersContainer.hpp"
#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Graphics/GPU/GPUUtils.hpp"

class GPUInstance: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    u32 requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType);

private:
private:
    GPUUniformBuffersContainer mGPUUniformBuffersContainer;
    u32 mBindingPointsIndexUniform = 0;
    u32 mBindingPointsIndexStorage = 0;
    i32 mMaxUniformBufferBindingPointsUniform = 0;
    i32 mMaxUniformBufferBindingPointsStorage = 0;

    struct Config {
        std::string Name;
        // Log::Level LogLevel;
        // GPUWindow::Config mWindow;
        Vulkan::Config mVulkan;
    };

public:
    OwnerPtr<GPUContext> mGPUContext;

public:
    RGET(GPUUniformBuffersContainer)
    CRGET(GPUUniformBuffersContainer)
};
REGISTER_CLASS(GPUInstance)
