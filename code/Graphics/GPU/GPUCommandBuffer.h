#pragma once

#include <vulkan/vulkan.h>

#include "Core/Minimal.hpp"
//namespace GPUAPI {

    class GPUCommandBuffer {
    private:
        static const VkAllocationCallbacks* ALLOCATOR;

    private:
        VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;

    public:
        const VkCommandBuffer getVkCommandBuffer() const;

        void init(VkCommandBuffer commandBuffer);

        bool begin(VkCommandBufferUsageFlags usageFlags = 0) const;

        bool end() const;

        void reset() const;
    };

// }
