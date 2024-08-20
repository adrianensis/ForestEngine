#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"

void GPUVertexBuffer::init(u32 attributeLocation, const GPUVertexBufferData& data, bool isStatic)
{
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
}

void GPUVertexBuffer::createBuffer()
{
//    mBufferId = GET_SYSTEM(GPUInterface).createBuffer(GPUBufferType::VERTEX);
    
    u32 sizeInPrimitiveTypes = mData.mGPUVariableData.mGPUDataType.getSizePrimitiveType();
//    GET_SYSTEM(GPUInterface).attribute(
        mAttributeLocation,
        sizeInPrimitiveTypes,
        mData.mGPUVariableData.mGPUDataType.mPrimitiveDataType,
        0/*mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes*/,
        0/*mPreviousOffsetInBytes*/,
        mData.mInstanceDivisor
    );
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

GPUVertexBuffer::GPUVertexBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice, GPUCommandPool* vulkanCommandPool)
        : vulkanPhysicalDevice(vulkanPhysicalDevice), vulkanDevice(vulkanDevice), vulkanCommandPool(vulkanCommandPool), buffer(vulkanPhysicalDevice, vulkanDevice) {}

const GPUBuffer& GPUVertexBuffer::getGPUBuffer() const {
    return buffer;
}

// bool GPUVertexBuffer::initialize(const std::vector<Vertex>& vertices) {
//     VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

//     GPUBuffer::Config stagingBufferConfig{};
//     stagingBufferConfig.Size = bufferSize;
//     stagingBufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//     stagingBufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

//     GPUBuffer stagingBuffer(vulkanPhysicalDevice, vulkanDevice);
//     if (!stagingBuffer.initialize(stagingBufferConfig)) {
//         CHECK_MSG(false,"Could not initialize staging buffer for vertex buffer");
//         return false;
//     }

//     GPUBuffer::Config bufferConfig{};
//     bufferConfig.Size = bufferSize;
//     bufferConfig.Usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
//     bufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

//     if (!buffer.initialize(bufferConfig)) {
//         CHECK_MSG(false,"Could not initialize vertex buffer");
//         return false;
//     }

//     stagingBuffer.setData((void*) vertices.data());
//     GPUBuffer::copy(stagingBuffer, buffer, *vulkanCommandPool, *vulkanDevice);
//     LOG("Copied vertices to vertex buffer");

//     stagingBuffer.terminate();
//     LOG("Terminated staging buffer for vertex buffer");

//     LOG("Initialized Vulkan vertex buffer");
//     return true;
// }
