#pragma once

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUCommandBuffer {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    public:
        explicit GPUCommandBuffer(VkCommandBuffer commandBuffer);

        const VkCommandBuffer getVkCommandBuffer() const;

        bool begin(VkCommandBufferUsageFlags usageFlags = 0) const;

        bool end() const;

        void reset() const;
    };

// }
