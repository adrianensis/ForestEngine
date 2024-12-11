#include "GPU/Core/GPUContext.hpp"
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

#ifdef ENGINE_ENABLE_PROFILER
#ifdef VK_EXT_calibrated_timestamps
    vulkanConfig.mOptionalExtensions.push_back(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME);
#endif
#endif

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
    vulkanCommandPool = new GPUCommandPool(vulkanDevice, getPtrToThis<GPUContext>());
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
    profilingCommandPool_ = new GPUCommandPool(vulkanDevice, getPtrToThis<GPUContext>());
    profilingCommandPool_->init();
    profilingCommandBuffer_ = profilingCommandPool_->allocateCommandBuffers(1)[0];

#ifdef VK_EXT_calibrated_timestamps

    if(vulkan->isExtensionAvailable(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME))
    {
        // NEXT: refactor extension loading into GPUUtils
        const char* functionName_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT";
        auto function_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT) vkGetInstanceProcAddr(vulkan->getGPUInstance(), functionName_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);
        if (function_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT == nullptr) {
            CHECK_MSG(false, "Could not look up address of extension function " + std::string(functionName_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT));
        }
        const char* functionName_vkGetCalibratedTimestampsEXT = "vkGetCalibratedTimestampsEXT";
        auto function_vkGetCalibratedTimestampsEXT = (PFN_vkGetCalibratedTimestampsEXT) vkGetInstanceProcAddr(vulkan->getGPUInstance(), functionName_vkGetCalibratedTimestampsEXT);
        if (function_vkGetCalibratedTimestampsEXT == nullptr) {
            CHECK_MSG(false, "Could not look up address of extension function " + std::string(functionName_vkGetCalibratedTimestampsEXT));
        }
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
    vulkan->terminate();
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