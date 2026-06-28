#include "GPU/Buffer/GPUIndicesBuffer.hpp"
#include "GPU/Core/GPUMemoryAllocator.hpp"

void GPUIndicesBuffer::init(GPUContext* gpuContext, const GPUDataType& gpuDataType, GPU::u32 size, bool isStatic)
{
    mGPUContext = gpuContext;
    mIsStatic = isStatic;
    mGPUDataType = gpuDataType;
    GPUBufferData gpuBufferData{};
    gpuBufferData.Size = mGPUDataType.mTypeSizeInBytes * size;
    gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    gpuBufferData.MemoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (!mBuffer.init(mGPUContext, gpuBufferData)) {
        CHECK_MSG(false,"Could not initialize staging mBuffer for index buffer");
    }
}

// void GPUIndicesBuffer::resize(GPU::u32 size)
// {
//     mBuffer.resize(size * mGPUDataType.mTypeSizeInBytes);
// }

const GPUBuffer& GPUIndicesBuffer::getGPUBuffer() const {
    return mBuffer;
}

bool GPUIndicesBuffer::setData(const void* data, GPU::u32 size) {
    VkDeviceSize bufferSize = size;

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = bufferSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    GPUBuffer stagingBuffer;//(vulkanPhysicalDevice, vulkanDevice);
    if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize staging buffer for index mBuffer");
        return false;
    }

    stagingBuffer.setData(data, size);
    GPUBuffer::copy(mGPUContext, stagingBuffer, mBuffer, nullptr);
    // LOG("Copied indices to index buffer");

    stagingBuffer.terminate();
    // LOG("Terminated staging buffer for index buffer");

    // LOG("Initialized Vulkan index buffer");
    return true;
}

void GPUIndicesBuffer::terminate() {
    mBuffer.terminate();
    // LOG("Terminated Vulkan index buffer");
}