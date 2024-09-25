#pragma once

#include "Graphics/GPU/Core/GPUPhysicalDevice.h"

class GPUDevice 
{
private:
    inline static const VkAllocationCallbacks* ALLOCATOR = VK_NULL_HANDLE;

private:
    Vulkan* mVulkan;
    GPUPhysicalDevice* mPhysicalDevice;
    VkDevice mDevice = VK_NULL_HANDLE;
    VkQueue mGraphicsQueue = VK_NULL_HANDLE;
    VkQueue mPresentQueue = VK_NULL_HANDLE;

public:
    GPUDevice(Vulkan* vulkan, GPUPhysicalDevice* vulkanPhysicalDevice);
    bool init();
    void terminate() const;
    void waitUntilIdle() const;

private:
    std::vector<VkDeviceQueueCreateInfo> getDeviceQueueCreateInfos(const GPUQueueFamilyIndices& queueFamilyIndices) const;
    bool createDevice(const std::vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos);
    bool findDeviceQueues(const GPUQueueFamilyIndices& queueFamilyIndices);
    VkQueue findDeviceQueue(uint32_t queueFamilyIndex) const;

public:
    GET(Vulkan)
    GET(PhysicalDevice)
    CRGET(Device)
    CRGET(GraphicsQueue)
    CRGET(PresentQueue)
};