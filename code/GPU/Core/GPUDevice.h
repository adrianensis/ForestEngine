#pragma once

#include "GPU/Core/GPUPhysicalDevice.h"

class GPUDevice 
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    GPUVulkanInstance* mGPUVulkanInstance = nullptr;
    GPUPhysicalDevice* mPhysicalDevice = nullptr;
    VkDevice mDevice = VK_NULL_HANDLE;
    VkQueue mGraphicsQueue = VK_NULL_HANDLE;
    VkQueue mPresentQueue = VK_NULL_HANDLE;
    inline static const GPU::f32 mQueuesPriority = 1.0;

public:
    GPUDevice(GPUVulkanInstance* gpuVulkanInstance, GPUPhysicalDevice* vulkanPhysicalDevice);
    bool init();
    void terminate() const;
    void waitUntilIdle() const;

private:
    std::vector<VkDeviceQueueCreateInfo> getDeviceQueueCreateInfos(const GPUQueueFamilyIndices& queueFamilyIndices) const;
    bool createDevice(const std::vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos);
    bool findDeviceQueues(const GPUQueueFamilyIndices& queueFamilyIndices);
    VkQueue findDeviceQueue(GPU::u32 queueFamilyIndex) const;

public:
    GET(GPUVulkanInstance)
    GET(PhysicalDevice)
    CRGET(Device)
    CRGET(GraphicsQueue)
    CRGET(PresentQueue)
};