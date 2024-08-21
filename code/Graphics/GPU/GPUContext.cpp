#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/GPU/GPUPhysicalDevice.h"
#include "Graphics/GPU/GPUDevice.h"
#include "Graphics/GPU/GPUSwapChain.h"
#include "Graphics/GPU/GPUCommandPool.h"
#include "Graphics/GPU/GPUCommandBuffer.h"

#include "Graphics/Window/WindowManager.hpp"

void GPUContext::init()
{
    Vulkan::Config vulkanConfig;
    vulkan = new Vulkan(vulkanConfig, GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer());
    vulkanPhysicalDevice = new GPUPhysicalDevice(vulkan);
    vulkanDevice = new GPUDevice(vulkan, vulkanPhysicalDevice);
    vulkanSwapChain = new GPUSwapChain(vulkanDevice, vulkanPhysicalDevice, vulkan, GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer());
    vulkanCommandPool = new GPUCommandPool(vulkanDevice, vulkanPhysicalDevice);

    if (!vulkan->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan");
    }
    if (!vulkanPhysicalDevice->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan physical device");
    }
    if (!vulkanDevice->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan device");
    }
    if (!vulkanSwapChain->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan swap chain");
    }
    if (!vulkanCommandPool->initialize()) 
    {
        CHECK_MSG(false, "Could not initialize Vulkan command pool");
    }
    vulkanCommandBuffers = vulkanCommandPool->allocateCommandBuffers(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vulkanCommandBuffers.empty())
    {
        CHECK_MSG(false, "Could not initialize Vulkan command buffers");
    }
}