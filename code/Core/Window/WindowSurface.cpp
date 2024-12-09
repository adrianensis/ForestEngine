#include "Core/Window/WindowSurface.hpp"
#include "Core/Window/Window.hpp"

VkSurfaceKHR WindowSurface::createSurface(VkInstance vulkan, Window* window, const VkAllocationCallbacks *allocator)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if(glfwCreateWindowSurface(vulkan, window->getGlfwWindow(), allocator, (VkSurfaceKHR*) &surface) != VK_SUCCESS)
    {
        CHECK_MSG(false, "Error creating surface!")
    }

    return surface;
};
