#include "GPU/Buffer/GPUIndicesBuffer.hpp"

void GPUIndicesBuffer::init(Ptr<GPUContext> gpuContext, const GPUDataType& gpuDataType, u32 size, bool isStatic)
{
    mGPUContext = gpuContext;
    mIsStatic = isStatic;
    mGPUDataType = gpuDataType;
    GPUBufferData gpuBufferData{};
    gpuBufferData.Size = mGPUDataType.mTypeSizeInBytes * size;
    gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!mBuffer.init(mGPUContext, gpuBufferData)) {
        CHECK_MSG(false,"Could not initialize staging mBuffer for index buffer");
    }
}

void GPUIndicesBuffer::resize(u32 size)
{
    mBuffer.resize(size * mGPUDataType.mTypeSizeInBytes);
}

const GPUBuffer& GPUIndicesBuffer::getGPUBuffer() const {
    return mBuffer;
}

bool GPUIndicesBuffer::setData(const void* data, u32 size) {
    VkDeviceSize bufferSize = size;

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = bufferSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    GPUBuffer stagingBuffer;//(vulkanPhysicalDevice, vulkanDevice);
    if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize staging buffer for index mBuffer");
        return false;
    }

    stagingBuffer.setData(data);
    GPUBuffer::copy(stagingBuffer, mBuffer, *mGPUContext->vulkanCommandPool, *mGPUContext->vulkanDevice);
    LOG("Copied indices to index buffer");

    stagingBuffer.terminate();
    LOG("Terminated staging buffer for index buffer");

    LOG("Initialized Vulkan index buffer");
    return true;
}

void GPUIndicesBuffer::terminate() {
    mBuffer.terminate();
    LOG("Terminated Vulkan index buffer");
}