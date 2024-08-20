#pragma once

#include "GPUPhysicalDevice.h"
#include "GPUDevice.h"
#include "GPUCommandPool.h"

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUBuffer {
    public:
        struct Config {
            VkDeviceSize Size = 0;
            VkBufferUsageFlags Usage = 0;
            VkMemoryPropertyFlags MemoryProperties = 0;
        };

    private:
        GPUPhysicalDevice* vulkanPhysicalDevice;
        GPUDevice* vulkanDevice;
        Config config;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    public:
        GPUBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice);

        const Config& getConfig() const;

        const VkBuffer getVkBuffer() const;

        const VkDeviceMemory getVkDeviceMemory() const;

        bool initialize(const Config& config);

        void terminate();

        void setData(void* data) const;

        static void copy(const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, const GPUCommandPool& commandPool, const GPUDevice& vulkanDevice);
    };
// }
