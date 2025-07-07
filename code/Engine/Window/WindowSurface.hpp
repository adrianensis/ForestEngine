#pragma once

#include <vulkan/vulkan.h>
#include "Core/CoreBase.hpp"

NS_BEGIN(Window)
class Window;
class WindowSurface
{
public:
    static VkSurfaceKHR createSurface(VkInstance vulkan, Window* window, const VkAllocationCallbacks *allocator);
};
NS_END