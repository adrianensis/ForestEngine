#include "GPU/Core/GPUContext.hpp"
#include "Core/Window/WindowSurface.hpp"

#include "Core/Window/WindowManager.hpp"
#include "GPU/GPUUtils.hpp"

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

#ifdef ENGINE_ENABLE_PROFILER
#ifdef VK_EXT_calibrated_timestamps
    vulkanConfig.mOptionalExtensions.push_back(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);
#endif
#endif

    gpuVulkanInstance = new GPUVulkanInstance(vulkanConfig);
    if (!gpuVulkanInstance->init())
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

    vulkanPhysicalDevice = new GPUPhysicalDevice(gpuVulkanInstance, gpuPhysicalDeviceData);
    if (!vulkanPhysicalDevice->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan physical device");
    }
    vulkanDevice = new GPUDevice(gpuVulkanInstance, vulkanPhysicalDevice);
    if (!vulkanDevice->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan device");
    }
    vulkanSwapChain = new GPUSwapChain(vulkanDevice, surface, GET_SYSTEM(WindowManager).getMainWindow()->getSizeInPixels());
    if (!vulkanSwapChain->init())
    {
        CHECK_MSG(false, "Could not initialize Vulkan swap chain");
    }
    vulkanCommandPool = new GPUCommandPool(getPtrToThis<GPUContext>());
    if (!vulkanCommandPool->init()) 
    {
        CHECK_MSG(false, "Could not initialize Vulkan command pool");
    }
    vulkanCommandBuffers = vulkanCommandPool->allocateCommandBuffers(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vulkanCommandBuffers.empty())
    {
        CHECK_MSG(false, "Could not initialize Vulkan command buffers");
    }

#ifdef ENGINE_ENABLE_PROFILER
    profilingCommandPool_ = new GPUCommandPool(getPtrToThis<GPUContext>());
    profilingCommandPool_->init();
    profilingCommandBuffer_ = profilingCommandPool_->allocateCommandBuffers(1)[0];

#ifdef VK_EXT_calibrated_timestamps

    if(gpuVulkanInstance->isExtensionAvailable(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME))
    {
        auto function_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = GPU_LOAD_EXTENSION_FUNCTION(getPtrToThis<GPUContext>(), vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);
        auto function_vkGetCalibratedTimestampsEXT = GPU_LOAD_EXTENSION_FUNCTION(getPtrToThis<GPUContext>(), vkGetCalibratedTimestampsEXT);
        mTracyContext = PROFILER_GPU_CONTEXT_CALIBRATED(vulkanPhysicalDevice->getPhysicalDevice(),
                                                vulkanDevice->getDevice(),
                                                vulkanDevice->getGraphicsQueue(),
                                                profilingCommandBuffer_->getVkCommandBuffer(),
                                                function_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT,
                                                function_vkGetCalibratedTimestampsEXT);
    }
    
#endif
    // If VK_EXT_calibrated_timestamps is not available or it has not been enabled, use the
    // uncalibrated Tracy context
    if (!mTracyContext)
    {
        mTracyContext = PROFILER_GPU_CONTEXT(
            vulkanPhysicalDevice->getPhysicalDevice(),
            vulkanDevice->getDevice(),
            vulkanDevice->getGraphicsQueue(),
            profilingCommandBuffer_->getVkCommandBuffer());
    }

    VULKAN_LOG_WARNING("Failed to create Tracy GPU CALIBRATED profiling context.")
    VULKAN_LOG_WARNING("Creating normal Tracy GPU profiling context instead.")
#endif
}

void GPUContext::terminate()
{
#ifdef ENGINE_ENABLE_PROFILER
    if (mTracyContext)
    {
        TracyVkDestroy(mTracyContext);
        profilingCommandPool_->terminate();
        delete profilingCommandPool_;
    }
#endif

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
    vulkanDevice->terminate();
    delete vulkanDevice;
    delete vulkanPhysicalDevice;
    vulkanCommandPool->terminate();
    vulkanCommandPool->terminate();
    delete vulkanCommandPool;
    gpuVulkanInstance->terminate();
    delete gpuVulkanInstance;
}

bool GPUContext::createSurface()
{
    surface = WindowSurface::createSurface(gpuVulkanInstance->getVkInstance(), GET_SYSTEM(WindowManager).getMainWindow().getInternalPointer(), ALLOCATOR);
    return true;
}

void GPUContext::destroySurface() const
{
    vkDestroySurfaceKHR(gpuVulkanInstance->getVkInstance(), surface, ALLOCATOR);
    VULKAN_LOG("Destroyed Vulkan window surface");
}