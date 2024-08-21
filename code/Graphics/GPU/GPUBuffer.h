#pragma once

#include "GPUPhysicalDevice.h"
#include "GPUDevice.h"
#include "GPUCommandPool.h"
#include "Graphics/GPU/GPUContext.hpp"

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
        Ptr<GPUContext> mGPUContext;
        Config config;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;

    public:
        const Config& getConfig() const;

        const VkBuffer getVkBuffer() const;

        const VkDeviceMemory getVkDeviceMemory() const;

        bool init(Ptr<GPUContext> gpuContext, const Config& config);

        void terminate();

        void setData(void* data) const;

        static void copy(const GPUBuffer& sourceBuffer, const GPUBuffer& destinationBuffer, const GPUCommandPool& commandPool, const GPUDevice& vulkanDevice);
    };
// }
