#pragma once

#include <vulkan/vulkan.h>
#include "vulkan/vulkan.hpp"

class Window;
class WindowSurface
{
public:
    static VkSurfaceKHR createSurface(VkInstance vulkan, Window* window, const VkAllocationCallbacks *allocator);
};
