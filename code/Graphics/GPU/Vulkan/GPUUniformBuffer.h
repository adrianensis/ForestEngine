#pragma once

#include "GPUBuffer.h"

namespace GPUAPI {

    class GPUUniformBuffer {
    private:
        GPUBuffer buffer;

    public:
        GPUUniformBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice);

        const GPUBuffer& getBuffer() const;

        bool initialize(uint32_t bufferSize);

        void terminate();

        void setData(void* data) const;
    };

}
