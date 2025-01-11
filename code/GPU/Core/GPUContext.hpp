#pragma once
#include "GPU/Core/GPUVulkanInstance.h"
#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUSwapChain.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Core/GPUCommandBuffer.h"

class GPUContext: public EnablePtrToThis
{
public:
    void init();
    void terminate();
private:
    void initializeSyncObjects();
    bool createSurface();
    void destroySurface() const;

public:
    inline static const u32 MAX_FRAMES_IN_FLIGHT = 2;

    GPUVulkanInstance* gpuVulkanInstance;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GPUSwapChain* vulkanSwapChain;
    GPUCommandPool* vulkanCommandPool;
    std::vector<GPUCommandBuffer*> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    u32 currentFrame = 0;

#ifdef ENGINE_ENABLE_PROFILER
    TracyVkCtx mTracyContext = nullptr;
    GPUCommandPool* profilingCommandPool_ = nullptr;
    GPUCommandBuffer* profilingCommandBuffer_ = nullptr;
#endif

private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
};