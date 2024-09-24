#include "Graphics/GPU/Core/GPUContext.hpp"

#include "Graphics/Window/WindowManager.hpp"

void GPUContext::init()
{
    VulkanConfig vulkanConfig;
    vulkan = new Vulkan(vulkanConfig, GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer());
    if (!vulkan->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan");
    }
    vulkanPhysicalDevice = new GPUPhysicalDevice(vulkan);
    if (!vulkanPhysicalDevice->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan physical device");
    }
    vulkanDevice = new GPUDevice(vulkan, vulkanPhysicalDevice);
    if (!vulkanDevice->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan device");
    }
    vulkanSwapChain = new GPUSwapChain(vulkanDevice, vulkan->getSurface(), GET_SYSTEM(WindowManager).getMainWindow()->getSizeInPixels());
    if (!vulkanSwapChain->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan swap chain");
    }
    vulkanCommandPool = new GPUCommandPool(vulkanDevice);
    if (!vulkanCommandPool->init()) 
    {
        CHECK_MSG(false, "Could not initialize Vulkan command pool");
    }
    vulkanCommandBuffers = vulkanCommandPool->allocateCommandBuffers(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vulkanCommandBuffers.empty())
    {
        CHECK_MSG(false, "Could not initialize Vulkan command buffers");
    }
}