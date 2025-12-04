#pragma once

#include "Core/Core.hpp"
#include <vulkan/vulkan.h>

class GPUContext;

class IGPUWindow
{
public:
    virtual std::vector<const char*> getRequiredGPUExtensions() const = 0;
    virtual Maths::Vector2 getGPUWindowSize() const = 0;
    virtual VkSurfaceKHR createSurface(GPUContext* gpuContext) const = 0;
};