#include "Graphics/GPU/GPUUniformBuffer.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

GPUVariableData GPUUniformBufferData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = HashedString(mInstanceName.get() + "." + data.mName.get());
    return data;
}

void GPUUniformBuffer::init(Ptr<GPUContext> gpuContext, u32 size, u32 bindingPoint, const GPUUniformBufferData& gpuBufferData, bool isStatic)
{
    mGPUContext = gpuContext;
	mGPUUniformBufferData = gpuBufferData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;
    mSize = size;

    initialize();
}

void GPUUniformBuffer::createBuffer()
{
//    mBufferId = GET_SYSTEM(GPUInterface).createBuffer(mGPUUniformBufferData.mType);
//    GET_SYSTEM(GPUInterface).bindUniformBufferToBindingPoint(mGPUUniformBufferData.mType, mBufferId, mBindingPoint);
    // mGPUPointer = glMapNamedBuffer(mBufferId, GL_READ_WRITE);

    // for (size_t i = 0; i < GPUContext::MAX_FRAMES_IN_FLIGHT; i++) {
    //     GPUUniformBuffer uniformBuffer(GET_SYSTEM(GPUInstance).vulkanPhysicalDevice, GET_SYSTEM(GPUInstance).vulkanDevice);
    //     if (!uniformBuffer.initialize(sizeof(f32) * 200)) {
    //         CHECK_MSG(false, "Could not initialize uniform buffer for frame [{}]");
    //     }
    //     uniformBuffers.push_back(uniformBuffer);
    // }
    // LOG("Initialized [{}] Vulkan uniform buffers");
}

// void GPUUniformBuffer::terminate()
// {
//     // glUnmapNamedBuffer(mBufferId);
// //    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);

//     for (GPUUniformBuffer& uniformBuffer : uniformBuffers)
//     {
//         uniformBuffer.terminate();
//     }
// }

void GPUUniformBuffer::checkMaxSize(u32 bytes) const
{
//    u32 maxBytes = GET_SYSTEM(GPUInterface).getMaxBytesInUniformBuffer(mGPUUniformBufferData.mType);
    // CHECK_MSG(bytes <= maxBytes, "Max bytes reached in Shared Buffer!");
}

const GPUBuffer& GPUUniformBuffer::getBuffer() const {
    return buffer;
}

bool GPUUniformBuffer::initialize() {
    GPUBuffer::Config bufferConfig{};
    bufferConfig.Size = mSize;
    switch (mGPUUniformBufferData.mType)
    {
    case GPUBufferType::UNIFORM:
        bufferConfig.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    case GPUBufferType::STORAGE:
        bufferConfig.Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        break;
    }
    bufferConfig.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (!buffer.init(mGPUContext, bufferConfig)) {
        CHECK_MSG(false,"Could not initialize uniform buffer");
        return false;
    }

    LOG("Initialized uniform buffer");
    return true;
}

void GPUUniformBuffer::terminate() {
    buffer.terminate();
}

void GPUUniformBuffer::setData(void* data) const {
    buffer.setData(data);
}