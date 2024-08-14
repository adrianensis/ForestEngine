#pragma once

#include "GPUPhysicalDevice.h"
#include "GPUDevice.h"
#include "GPUCommandPool.h"
#include "GPUBuffer.h"
#include "Vertex.h"

#include <vulkan/vulkan.h>

namespace GPUAPI {

    class GPUVertexBuffer {
    private:
        GPUPhysicalDevice* vulkanPhysicalDevice;
        GPUDevice* vulkanDevice;
        GPUCommandPool* vulkanCommandPool;
        GPUBuffer buffer;

    public:
        GPUVertexBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice, GPUCommandPool* vulkanCommandPool);

        const GPUBuffer& getGPUBuffer() const;

        bool initialize(const std::vector<Vertex>& vertices);

        void terminate();
    };
}
