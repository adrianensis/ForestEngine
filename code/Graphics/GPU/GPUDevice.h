#pragma once

#include "Graphics/GPU/GPUPhysicalDevice.h"

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUDevice {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        Vulkan* vulkan;
        GPUPhysicalDevice* vulkanPhysicalDevice;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

    public:
        GPUDevice(Vulkan* vulkan, GPUPhysicalDevice* vulkanPhysicalDevice);

        const VkDevice getDevice() const;

        const VkQueue getGraphicsQueue() const;

        const VkQueue getPresentQueue() const;

        bool initialize();

        void terminate() const;

        void waitUntilIdle() const;

    private:
        std::vector<VkDeviceQueueCreateInfo> getDeviceQueueCreateInfos(const QueueFamilyIndices& queueFamilyIndices) const;

        bool createDevice(const std::vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos);

        bool findDeviceQueues(const QueueFamilyIndices& queueFamilyIndices);

        VkQueue findDeviceQueue(uint32_t queueFamilyIndex) const;
    };

// }