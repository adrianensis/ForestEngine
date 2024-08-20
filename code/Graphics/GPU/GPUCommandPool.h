#pragma once

#include "GPUPhysicalDevice.h"
#include "GPUDevice.h"
#include "GPUCommandBuffer.h"

#include <vulkan/vulkan.h>
#include <vector>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUCommandPool {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        GPUPhysicalDevice* vulkanPhysicalDevice;
        GPUDevice* vulkanDevice;
        VkCommandPool vkCommandPool = VK_NULL_HANDLE;

    public:
        GPUCommandPool(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice);

        const VkCommandPool getVkCommandPool() const;

        bool initialize();

        void terminate();

        std::vector<GPUCommandBuffer> allocateCommandBuffers(uint32_t count) const;

        void free(const GPUCommandBuffer& commandBuffer) const;
    };
// }
