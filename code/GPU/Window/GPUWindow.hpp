#pragma once

#include "Core/Core.hpp"
#include <vulkan/vulkan.h>

class GPUContext;

class GPUWindowSize
{
public:
    float x = 0;
    float y = 0;
};

class IGPUWindow
{
public:
    virtual std::vector<const char*> getRequiredGPUExtensions() const = 0;
    virtual GPUWindowSize getGPUWindowSize() const = 0;
    virtual VkSurfaceKHR createSurface(GPUContext* gpuContext) const = 0;
};