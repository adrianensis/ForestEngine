#include "Core/Window/WindowSurface.hpp"
#include "Core/Window/Window.hpp"

NS_BEGIN(Core)
VkSurfaceKHR WindowSurface::createSurface(VkInstance gpuVulkanInstance, Window* window, const VkAllocationCallbacks *allocator)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if(glfwCreateWindowSurface(gpuVulkanInstance, window->getGlfwWindow(), allocator, (VkSurfaceKHR*) &surface) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Error creating surface!")
    }

    return surface;
};
NS_END