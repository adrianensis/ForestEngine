#include "Graphics/GPU/Buffer/GPUIndicesBuffer.hpp"

void GPUIndicesBuffer::init(Ptr<GPUContext> gpuContext, const GPUDataType& gpuDataType, bool isStatic)
{
    mGPUContext = gpuContext;
    mIsStatic = isStatic;
    mGPUDataType = gpuDataType;
//    mBufferId = GET_SYSTEM(GPUInterface).createBuffer(GPUBufferType::INDEX);
    GPUBufferData gpuBufferData{};
    gpuBufferData.Size = sizeof(f32) * 10000; //bufferSize;;
    gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!buffer.init(mGPUContext, gpuBufferData)) {
        CHECK_MSG(false,"Could not initialize staging buffer for index buffer");
    }
}

void GPUIndicesBuffer::resize(u32 size)
{
//    GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::INDEX, mBufferId, mGPUDataType.mTypeSizeInBytes, size, mIsStatic);
}

// void GPUIndicesBuffer::terminate()
// {
// //    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
// }

const GPUBuffer& GPUIndicesBuffer::getGPUBuffer() const {
    return buffer;
}

bool GPUIndicesBuffer::setData(const void* data, u32 size) {
    VkDeviceSize bufferSize = size;

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = bufferSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    GPUBuffer stagingBuffer;//(vulkanPhysicalDevice, vulkanDevice);
    if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize staging buffer for index buffer");
        return false;
    }

    // GPUBufferData gpuBufferData{};
    // gpuBufferData.Size = bufferSize;
    // gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    // gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    // if (!buffer.init(mGPUContext, gpuBufferData)) {
    //     CHECK_MSG(false,"Could not initialize index buffer");
    //     return false;
    // }

    stagingBuffer.setData(data);
    GPUBuffer::copy(stagingBuffer, buffer, *mGPUContext->vulkanCommandPool, *mGPUContext->vulkanDevice);
    LOG("Copied indices to index buffer");

    stagingBuffer.terminate();
    LOG("Terminated staging buffer for index buffer");

    LOG("Initialized Vulkan index buffer");
    return true;
}

void GPUIndicesBuffer::terminate() {
    buffer.terminate();
    LOG("Terminated Vulkan index buffer");
}