#include "GPU/Buffer/GPUVertexBuffer.hpp"

void GPUVertexBuffer::init(GPUContext* gpuContext, GPU::u32 attributeLocation, const GPUVertexBufferData& data, GPU::u32 size, bool isStatic)
{
    mGPUContext = gpuContext;
	mData = data;
    mAttributeLocation = attributeLocation;
    mIsStatic = isStatic;

    // sizeInPrimitiveTypes: size of the object divided in primitive types
    // ex: Maths::Vector3 -> 3 floats
    GPU::u32 sizeInPrimitiveTypes = mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType();
    // accumulative offset in bytes
    GPU::u32 primitiveTypeSizeInBytes = mData.mGPUVariableData.mGPUDataType.getPrimitiveTypeSizeInBytes();
    mPreviousOffsetInBytes = mPreviousOffsetInBytes + sizeInPrimitiveTypes * primitiveTypeSizeInBytes;
    mAttributeOffset += 1;

    GPUBufferData gpuBufferData{};
    gpuBufferData.Size = mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes * size;
    gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!mBuffer.init(mGPUContext, gpuBufferData)) {
        CHECK_MSG(false,"Could not initialize vertex buffer");
    }
}

void GPUVertexBuffer::terminate()
{
    mBuffer.terminate();
}

// void GPUVertexBuffer::resize(GPU::u32 size)
// {
//     mBuffer.resize(mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes * size);
// }

GPU::u32 GPUVertexBuffer::getAttributeLocation() const
{
    return mAttributeLocation;
}

GPU::u32 GPUVertexBuffer::getAttributeLocationWithOffset() const
{
    return getAttributeLocation() + mAttributeOffset;
}

const GPUBuffer& GPUVertexBuffer::getGPUBuffer() const {
    return mBuffer;
}

bool GPUVertexBuffer::setData(const void* data, GPU::u32 size, VkCommandBuffer* commandBuffer)
{
    PROFILER_CPU_NAMED(vertex_buffer_set_data)

    VkDeviceSize bufferSize = size;

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = bufferSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    GPUBuffer stagingBuffer;
    if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize staging buffer for vertex mBuffer");
        return false;
    }

    stagingBuffer.setData(data, size);
    GPUBuffer::copy(mGPUContext, stagingBuffer, mBuffer, nullptr);
    stagingBuffer.terminate();

    return true;
}