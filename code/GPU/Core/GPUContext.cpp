#include "GPU/Core/GPUContext.hpp"
#include "Core/Window/WindowSurface.hpp"

#include "Core/Window/WindowManager.hpp"
#include "GPU/Core/GPULog.h"

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
    vulkanCommandPool = OwnerPtr<GPUCommandPool>::newObject();
    if (!vulkanCommandPool->init(this, 0)) 
    {
        CHECK_MSG(false, "Could not initialize Vulkan command pool");
    }
    vulkanCommandPoolSingleUse = OwnerPtr<GPUCommandPool>::newObject();
    if (!vulkanCommandPoolSingleUse->init(this, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)) 
    {
        CHECK_MSG(false, "Could not initialize Vulkan command pool Single Use");
    }
    vulkanCommandBuffers = vulkanCommandPool->allocateCommandBuffers(GPUContext::MAX_FRAMES_IN_FLIGHT);
    if (vulkanCommandBuffers.empty())
    {
        CHECK_MSG(false, "Could not initialize Vulkan command buffers");
    }

#ifdef ENGINE_ENABLE_PROFILER
    profilingCommandPool = OwnerPtr<GPUCommandPool>::newObject();
    profilingCommandPool->init(this, 0);
    profilingCommandBuffer_ = profilingCommandPool->allocateCommandBuffers(1)[0];

#ifdef VK_EXT_calibrated_timestamps

    if(gpuVulkanInstance->isExtensionAvailable(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME))
    {
        auto function_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = GPU_LOAD_EXTENSION_FUNCTION(vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);
        auto function_vkGetCalibratedTimestampsEXT = GPU_LOAD_EXTENSION_FUNCTION(vkGetCalibratedTimestampsEXT);
        mTracyContext = PROFILER_GPU_CONTEXT_CALIBRATED(vulkanPhysicalDevice->getPhysicalDevice(),
                                                vulkanDevice->getDevice(),
                                                vulkanDevice->getGraphicsQueue(),
                                                profilingCommandBuffer_.getVkCommandBuffer(),
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
            profilingCommandBuffer_.getVkCommandBuffer());
    }

    GPU_LOG_WARNING("Failed to create Tracy GPU CALIBRATED profiling context.")
    GPU_LOG_WARNING("Creating normal Tracy GPU profiling context instead.")
#endif

    initializeSyncObjects();
}

void GPUContext::initializeSyncObjects()
{
    imageAvailableSemaphores.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(GPUContext::MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Create the fence in the signaled state, so that the first call to vkWaitForFences() returns immediately since the fence is already signaled
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &imageAvailableSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'image available' semaphore for frame [{}]", i);
        }
        if (vkCreateSemaphore(vulkanDevice->getDevice(), &semaphoreInfo, allocationCallbacks, &renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'render finished' semaphore for frame [{}]", i);
        }
        if (vkCreateFence(vulkanDevice->getDevice(), &fenceInfo, allocationCallbacks, &inFlightFences[i]) != VK_SUCCESS)
        {
            CHECK_MSG(false, "Could not create 'in flight' fence for frame [{}]", i);
        }
    }
    LOG("Created Vulkan sync objects (semaphores & fences)");
}

void GPUContext::terminate()
{
#ifdef ENGINE_ENABLE_PROFILER
    if (mTracyContext)
    {
        TracyVkDestroy(mTracyContext);
        profilingCommandPool->freeCommandBuffer(profilingCommandBuffer_);
        profilingCommandPool->terminate();
        profilingCommandPool.invalidate();
    }
#endif

    // Wait for ALL operations
    vulkanDevice->waitUntilIdle();    

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
    destroySurface();
    FOR_RANGE(i, 0, GPUContext::MAX_FRAMES_IN_FLIGHT)
    {
        vulkanCommandPool->freeCommandBuffer(vulkanCommandBuffers[i]);
    }
    vulkanCommandPool->terminate();
    vulkanCommandPool.invalidate();
    vulkanCommandPoolSingleUse->terminate();
    vulkanCommandPoolSingleUse.invalidate();
    vulkanDevice->terminate();
    delete vulkanDevice;
    delete vulkanPhysicalDevice;
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
    GPU_LOG("Destroyed Vulkan window surface");
}

/*
    * TODO
    *
    * All of the helper functions that submit commands so far have been set up to execute synchronously by waiting for the queue to become idle.
    *
    * For practical applications it is recommended to combine these operations in a single command buffer and execute them asynchronously for higher throughput,
    * especially the transitions and copy in the createTextureImage function.
    *
    * Try to experiment with this by creating a setupCommandBuffer that the helper functions record commands into,
    * and add a flushSetupCommands to execute the commands that have been recorded so far.
    *
    * It's best to do this after the texture mapping works to check if the texture resources are still set up correctly.
    */
VkCommandBuffer GPUContext::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vulkanCommandPoolSingleUse->getVkCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vulkanDevice->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void GPUContext::endSingleTimeCommands(VkCommandBuffer commandBuffer, VkFence fence)
{
#if defined(ENGINE_ENABLE_PROFILER)
  PROFILER_GPU_COLLECT(mTracyContext, commandBuffer);
#endif

    PROFILER_CPU()

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if(fence != VK_NULL_HANDLE)
    {
        // After waiting, we need to manually reset the fence to the unsignaled state
        constexpr u32 fenceCount = 1;
        vkResetFences(vulkanDevice->getDevice(), fenceCount, &fence);
    }

    constexpr u32 submitCount = 1;
    vkQueueSubmit(vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, fence);

    // INFO: if no fence, wait queue idle
    if(fence == VK_NULL_HANDLE)
    {
        PROFILER_CPU_NAMED(wait_queue_idle)
        vkQueueWaitIdle(vulkanDevice->getGraphicsQueue());
    }
    else
    {
            // Wait until the previous frame has finished
        constexpr u32 fenceCount = 1;
        constexpr VkBool32 waitForAllFences = VK_TRUE;
        constexpr uint64_t waitForFenceTimeout = UINT64_MAX;
        VkResult waitResult = vkWaitForFences(vulkanDevice->getDevice(), fenceCount, &fence, waitForAllFences, waitForFenceTimeout);
    }

    vkFreeCommandBuffers(vulkanDevice->getDevice(), vulkanCommandPoolSingleUse->getVkCommandPool(), submitInfo.commandBufferCount, &commandBuffer);
}

void GPUContext::drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance)
{
    vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

u32 GPUContext::frameAcquisition()
{
    PROFILER_CPU()

    u32 swapChainImageIndex = 0;

    waitForFence(currentFrame);

    // Acquire an image from the swap chain
    VkFence acquireNextImageFence = VK_NULL_HANDLE;
    constexpr uint64_t acquireNextImageTimeout = UINT64_MAX;
    VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    VkResult acquireNextImageResult = vkAcquireNextImageKHR(
            vulkanDevice->getDevice(),
            vulkanSwapChain->getSwapChain(),
            acquireNextImageTimeout,
            imageAvailableSemaphore,
            acquireNextImageFence,
            &swapChainImageIndex
    );

    currentSwapChainImageIndex = swapChainImageIndex;
    
    // VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the surface and can no longer be used for rendering. Usually happens after a window resize.
    if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        //recreateRenderingObjects();
        CHECK_MSG(false, "TODO: implement window resize");
    }
    // VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present to the surface, but the surface properties are no longer matched exactly.
    if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR)
    {
        CHECK_MSG(false, "Could not acquire swap chain image");
    }

    // After waiting, we need to manually reset the fence to the unsignaled state
    constexpr u32 fenceCount = 1;
    VkFence inFlightFence = inFlightFences[currentFrame];
    vkResetFences(vulkanDevice->getDevice(), fenceCount, &inFlightFence);

    return swapChainImageIndex;
}

void GPUContext::waitForFence(u32 frameIndex)
{
    // Wait until the previous frame has finished
    constexpr u32 fenceCount = 1;
    constexpr VkBool32 waitForAllFences = VK_TRUE;
    constexpr uint64_t waitForFenceTimeout = UINT64_MAX;
    VkFence inFlightFence = inFlightFences[frameIndex % GPUContext::MAX_FRAMES_IN_FLIGHT];
    VkResult waitResult = vkWaitForFences(vulkanDevice->getDevice(), fenceCount, &inFlightFence, waitForAllFences, waitForFenceTimeout);
}

void GPUContext::commandSubmission()
{
    PROFILER_CPU()
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const GPUCommandBuffer& vulkanCommandBuffer = vulkanCommandBuffers[currentFrame];
    VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.getVkCommandBuffer();
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.commandBufferCount = 1;

    // Wait with writing colors to the image until it's available
    VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentFrame];
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;

    // Which semaphores to signal once the command buffer(s) have finished execution
    VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    // Submit recorded graphics commands
    constexpr u32 submitCount = 1;
    VkFence inFlightFence = inFlightFences[currentFrame];
    if (vkQueueSubmit(vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, inFlightFence) != VK_SUCCESS) {
        GPU_LOG_ERROR("Could not submit to graphics queue")
        CHECK_MSG(false, "Could not submit to graphics queue")
    }
}

void GPUContext::framePresentation(const std::vector<u32>& imageIndices)
{
    PROFILER_CPU()
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Which semaphores to wait on before presentation can happen
    VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.waitSemaphoreCount = 1;

    // Which swap chain to present image to
    VkSwapchainKHR swapChains[] = {vulkanSwapChain->getSwapChain()};
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndices.data();
    presentInfo.swapchainCount = imageIndices.size();

    // Present image to swap chain
    {
        PROFILER_CPU_NAMED(vkQueuePresentKHR)
        VkResult presentResult = vkQueuePresentKHR(vulkanDevice->getPresentQueue(), &presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR /*|| windowResized*/) {
            // windowResized = false;
            // recreateRenderingObjects();
        } else if (presentResult != VK_SUCCESS) {
            GPU_LOG_ERROR("Could not present image to swap chain")
            CHECK_MSG(false, "Could not present image to swap chain")
        }
    }
}

PFN_vkVoidFunction GPUContext::loadExtensionFunctionInternal(const char* extensionFunctionName)
{
    GPU_LOG("Loading Extension Function: ")
    GPU_LOG(extensionFunctionName)
    const char* functionName = TO_STRING(extensionFunctionName);\
    auto function = vkGetInstanceProcAddr(gpuVulkanInstance->getVkInstance(), functionName);
    if (function == nullptr)
    {
        CHECK_MSG(false, "Could not look up address of extension function!")
    }
    return function;
}