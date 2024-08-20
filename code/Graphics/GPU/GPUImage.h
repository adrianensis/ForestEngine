#pragma once

#include "GPUDevice.h"

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUImage {
    public:
        struct Config {
            uint32_t Width;
            uint32_t Height;
            uint32_t MipLevels;
            VkFormat Format;
            VkImageTiling Tiling;
            VkImageUsageFlags Usage;
            VkMemoryPropertyFlags MemoryProperties;
            VkImageLayout Layout;
            VkSampleCountFlagBits SampleCount;
        };

    private:
        GPUPhysicalDevice* vulkanPhysicalDevice;
        GPUDevice* vulkanDevice;
        VkImage vkImage = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    public:
        GPUImage(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice);

        VkImage getVkImage() const;

        bool initialize(const Config& config);

        void terminate();
    };

// }
