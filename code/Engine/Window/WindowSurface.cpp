#include "Engine/Window/WindowSurface.hpp"
#include "Engine/Window/Window.hpp"

NS_BEGIN(Window)
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