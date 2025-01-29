#pragma once
#include "GPU/Core/GPUVulkanInstance.h"
#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUSwapChain.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Core/GPUCommandBuffer.h"

#define GPU_LOAD_EXTENSION_FUNCTION(extensionFunctionName) \
loadExtensionFunction<PFN_##extensionFunctionName>(TO_STRING(extensionFunctionName));

class GPUContext: public EnableWeakPtrToThis
{
public:
    void init();
    void terminate();

    template<class T>
    T loadExtensionFunction(const char* extensionFunctionName)
    {
        return (T) loadExtensionFunctionInternal(extensionFunctionName);
    }

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void drawIndexed(VkCommandBuffer commandBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, i32 vertexOffset, u32 firstInstance);

    u32 frameAcquisition();
    void waitForFence(u32 frameIndex);
    void commandSubmission();
    void framePresentation(const std::vector<u32>& imageIndices);

private:
    void initializeSyncObjects();
    bool createSurface();
    void destroySurface() const;
    PFN_vkVoidFunction loadExtensionFunctionInternal(const char* extensionFunctionName);

public:
    inline static const u32 MAX_FRAMES_IN_FLIGHT = 2;

    GPUVulkanInstance* gpuVulkanInstance;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GPUSwapChain* vulkanSwapChain;
    OwnerPtr<GPUCommandPool> vulkanCommandPool;
    OwnerPtr<GPUCommandPool> vulkanCommandPoolSingleUse;
    std::vector<GPUCommandBuffer> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    u32 currentFrame = 0;
    u32 currentSwapChainImageIndex = 0;

#ifdef ENGINE_ENABLE_PROFILER
    TracyVkCtx mTracyContext = nullptr;
    OwnerPtr<GPUCommandPool> profilingCommandPool;
    GPUCommandBuffer profilingCommandBuffer_;
#endif

private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;
};