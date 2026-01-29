#pragma once
#include "GPU/Core/GPUVulkanInstance.h"
#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUSwapChain.h"
#include "GPU/Core/GPUCommandPool.h"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Core/GPUDefinitions.h"
class IGPUWindow;

#define GPU_EXTENSION_FUNCTION_TYPE(extensionFunctionName) PFN_##extensionFunctionName

#define GPU_LOAD_EXTENSION_FUNCTION(extensionFunctionName) \
loadExtensionFunction<GPU_EXTENSION_FUNCTION_TYPE(extensionFunctionName)>(TO_STRING(extensionFunctionName));

class GPUContext
{
public:
    void init(IGPUWindow* gpuWindow);
    void terminate();

    template<class T>
    T loadExtensionFunction(const char* extensionFunctionName)
    {
        return (T) loadExtensionFunctionInternal(extensionFunctionName);
    }

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkFence fence);

    void drawIndexed(VkCommandBuffer commandBuffer, Core::u32 indexCount, Core::u32 instanceCount, Core::u32 firstIndex, Core::i32 vertexOffset, Core::u32 firstInstance);

    Core::u32 frameAcquisition();
    void waitForFence(Core::u32 frameIndex);
    void commandSubmission();
    void framePresentation(const std::vector<Core::u32>& imageIndices);

    void setWindowResized();
    void recreateRenderingObjects();

    Core::u32 requestUniformBufferBindingPoint(GPUBufferType gpuUniformBufferType);

private:
    void initializeSyncObjects();
    bool createSurface();
    void destroySurface() const;
    PFN_vkVoidFunction loadExtensionFunctionInternal(const char* extensionFunctionName);
    
public:
    inline static const Core::u32 MAX_FRAMES_IN_FLIGHT = 2;
    
    GPUVulkanInstance* gpuVulkanInstance;
    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    GPUSwapChain* vulkanSwapChain;
    Core::OwnerPtr<GPUCommandPool> vulkanCommandPool;
    Core::OwnerPtr<GPUCommandPool> vulkanCommandPoolSingleUse;
    std::vector<GPUCommandBuffer> vulkanCommandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    Core::u32 currentFrame = 0;
    Core::u32 currentSwapChainImageIndex = 0;
    bool mWindowResized = false;
    IGPUWindow* mGPUWindow = nullptr;
    
    #ifdef ENGINE_ENABLE_PROFILER
    TracyVkCtx mTracyContext = nullptr;
    Core::OwnerPtr<GPUCommandPool> profilingCommandPool;
    GPUCommandBuffer profilingCommandBuffer_;
    #endif
    
    GPU_EXTENSION_FUNCTION_TYPE(vkCmdSetStencilOpEXT) function_vkCmdSetStencilOpEXT;
    GPU_EXTENSION_FUNCTION_TYPE(vkCmdSetStencilTestEnableEXT) function_vkCmdSetStencilTestEnableEXT;
    
private:
    Core::u32 mBindingPointsIndexUniform = 0;
    Core::u32 mBindingPointsIndexStorage = 0;
    Core::i32 mMaxUniformBufferBindingPointsUniform = 0;
    Core::i32 mMaxUniformBufferBindingPointsStorage = 0;
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

};