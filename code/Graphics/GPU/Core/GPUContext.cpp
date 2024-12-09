#include "Graphics/GPU/Core/GPUContext.hpp"
#include "Core/Window/WindowSurface.hpp"

#include "Core/Window/WindowManager.hpp"

void GPUContext::init()
{
    VulkanConfig vulkanConfig;
    vulkanConfig.mRequiredExtensions = GET_SYSTEM(WindowManager).getMainWindow()->getRequiredExtensions();
    if (Environment::mPlatform == Environment::Platform::MACOS) {
        vulkanConfig.mRequiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
    if (vulkanConfig.ValidationLayersEnabled) {
        vulkanConfig.mRequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    vulkan = new Vulkan(vulkanConfig);
    if (!vulkan->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan");
    }
    if (!createSurface()) {
        CHECK_MSG(false,"Could not create Vulka window surface");
    }

    GPUPhysicalDeviceData gpuPhysicalDeviceData
    {
        surface
    };

    vulkanPhysicalDevice = new GPUPhysicalDevice(vulkan, gpuPhysicalDeviceData);
    if (!vulkanPhysicalDevice->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan physical device");
    }
    vulkanDevice = new GPUDevice(vulkan, vulkanPhysicalDevice);
    if (!vulkanDevice->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan device");
    }
    vulkanSwapChain = new GPUSwapChain(vulkanDevice, surface, GET_SYSTEM(WindowManager).getMainWindow()->getSizeInPixels());
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

void GPUContext::terminate()
{
    destroySurface();

    // terminateSyncObjects
    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vulkanDevice->getDevice(), renderFinishedSemaphores[i], allocationCallbacks);
        vkDestroySemaphore(vulkanDevice->getDevice(), imageAvailableSemaphores[i], allocationCallbacks);
        vkDestroyFence(vulkanDevice->getDevice(), inFlightFences[i], allocationCallbacks);
    }
    LOG("Destroyed Vulkan sync objects (semaphores & fences)");

    vulkanSwapChain->terminate();
    delete vulkanSwapChain;
    delete vulkanDevice;
    delete vulkanPhysicalDevice;
    delete vulkanCommandPool;
    delete vulkan;
}

bool GPUContext::createSurface()
{
    surface = WindowSurface::createSurface(vulkan->getGPUInstance(), GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer(), ALLOCATOR);
    return true;
}

void GPUContext::destroySurface() const
{
    vkDestroySurfaceKHR(vulkan->getGPUInstance(), surface, ALLOCATOR);
    VULKAN_LOG("Destroyed Vulkan window surface");
}