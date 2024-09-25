#include "Graphics/GPU/Buffer/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

void GPUVertexBuffer::init(Ptr<GPUContext> gpuContext, u32 attributeLocation, const GPUVertexBufferData& data, bool isStatic)
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
    gpuBufferData.Size = sizeof(f32) * 10000; //bufferSize;
    gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!buffer.init(mGPUContext, gpuBufferData)) {
        CHECK_MSG(false,"Could not initialize vertex buffer");
    }
}

void GPUVertexBuffer::createBuffer()
{    
    u32 sizeInPrimitiveTypes = mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType();
}

void GPUVertexBuffer::terminate()
{
//    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
    buffer.terminate();
}

void GPUVertexBuffer::resize(u32 size)
{
//	GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::VERTEX, mBufferId, mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, size, mIsStatic);
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
    return buffer;
}

bool GPUVertexBuffer::setData(const void* data, u32 size)
{
    VkDeviceSize bufferSize = size;
    // VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    GPUBufferData stagingBufferConfig{};
    stagingBufferConfig.Size = bufferSize;
    stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    GPUBuffer stagingBuffer;
    if (!stagingBuffer.init(mGPUContext, stagingBufferConfig)) {
        CHECK_MSG(false,"Could not initialize staging buffer for vertex buffer");
        return false;
    }

    // GPUBufferData gpuBufferData{};
    // gpuBufferData.Size = bufferSize;
    // gpuBufferData.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    // gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    // if (!buffer.init(mGPUContext, gpuBufferData)) {
    //     CHECK_MSG(false,"Could not initialize vertex buffer");
    //     return false;
    // }

    stagingBuffer.setData(data);
    GPUBuffer::copy(stagingBuffer, buffer, *mGPUContext->vulkanCommandPool, *mGPUContext->vulkanDevice);
    LOG("Copied vertices to vertex buffer");

    stagingBuffer.terminate();
    LOG("Terminated staging buffer for vertex buffer");

    LOG("Initialized Vulkan vertex buffer");
    return true;
}
