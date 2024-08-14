#pragma once

#include "GPUPhysicalDevice.h"
#include "GPUDevice.h"
#include "GPUCommandPool.h"
#include "GPUBuffer.h"

#include <vulkan/vulkan.h>

namespace GPUAPI {

    class GPUIndexBuffer {
    private:
        GPUPhysicalDevice* vulkanPhysicalDevice;
        GPUDevice* vulkanDevice;
        GPUCommandPool* vulkanCommandPool;
        GPUBuffer buffer;

    public:
        GPUIndexBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice, GPUCommandPool* vulkanCommandPool);

        const GPUBuffer& getGPUBuffer() const;

        bool initialize(const std::vector<uint32_t>& indices);

        void terminate();
    };
}
