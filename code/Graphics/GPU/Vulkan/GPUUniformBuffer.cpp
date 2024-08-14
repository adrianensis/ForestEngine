#include "GPUUniformBuffer.h"
#include "Log.h"

namespace GPUAPI {

    GPUUniformBuffer::GPUUniformBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice) : buffer(vulkanPhysicalDevice, vulkanDevice) {}

    const GPUBuffer& GPUUniformBuffer::getBuffer() const {
        return buffer;
    }

    bool GPUUniformBuffer::initialize(uint32_t bufferSize) {
        GPUBuffer::Config bufferConfig{};
        bufferConfig.Size = bufferSize;
        bufferConfig.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        if (!buffer.initialize(bufferConfig)) {
            VD_LOG_ERROR("Could not initialize uniform buffer");
            return false;
        }

        VD_LOG_INFO("Initialized uniform buffer");
        return true;
    }

    void GPUUniformBuffer::terminate() {
        buffer.terminate();
    }

    void GPUUniformBuffer::setData(void* data) const {
        buffer.setData(data);
    }

}
