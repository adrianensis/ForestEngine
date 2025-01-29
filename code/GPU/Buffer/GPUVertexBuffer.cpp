#include "GPU/Buffer/GPUVertexBuffer.hpp"

void GPUVertexBuffer::init(Ptr<GPUContext> gpuContext, u32 attributeLocation, const GPUVertexBufferData& data, u32 size, bool isStatic)
{
    mGPUContext = gpuContext;
	mData = data;
    mAttributeLocation = attributeLocation;
    mIsStatic = isStatic;

    // sizeInPrimitiveTypes: size of the object divided in primitive types
    // ex: Vector3 -> 3 floats
    u32 sizeInPrimitiveTypes = mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType();
//    // GET_SYSTEM(GPUInterface).attribute(getAttributeLocationWithOffset(), sizeInPrimitiveTypes, primitiveType, mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, mPreviousOffsetInBytes, mData.mInstanceDivisor);
    // accumulative offset in bytes
    u32 primitiveTypeSizeInBytes = mData.mGPUVariableData.mGPUDataType.getPrimitiveTypeSizeInBytes();
    mPreviousOffsetInBytes = mPreviousOffsetInBytes + sizeInPrimitiveTypes * primitiveTypeSizeInBytes;
    mAttributeOffset += 1;

    GPUBufferData gpuBufferData{};
    // TODO: remove magic numbers on all Buffer classes, start small, then resize if needed
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

void GPUVertexBuffer::resize(u32 size)
{
    mBuffer.resize(mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes * size);
}

u32 GPUVertexBuffer::getAttributeLocation() const
{
    return mAttributeLocation;
}

u32 GPUVertexBuffer::getAttributeLocationWithOffset() const
{
    return getAttributeLocation() + mAttributeOffset;
}

const GPUBuffer& GPUVertexBuffer::getGPUBuffer() const {
    return mBuffer;
}

bool GPUVertexBuffer::setData(const void* data, u32 size)
{
    PROFILER_CPU_NAMED(vertex_buffer_set_data)
    VkDeviceSize bufferSize = size;

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = bufferSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    GPUBuffer stagingBuffer;
    if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize staging buffer for vertex buffer");
        return false;
    }

    stagingBuffer.setData(data, size);
    GPUBuffer::copy(mGPUContext, stagingBuffer, mBuffer);
    LOG("Copied vertices to vertex buffer");

    stagingBuffer.terminate();
    LOG("Terminated staging buffer for vertex buffer");

    LOG("Initialized Vulkan vertex buffer");
    return true;
}
