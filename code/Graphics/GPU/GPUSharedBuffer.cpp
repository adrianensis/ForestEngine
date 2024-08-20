#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"

GPUVariableData GPUSharedBufferData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = HashedString(mInstanceName.get() + "." + data.mName.get());
    return data;
}

void GPUSharedBuffer::init(u32 bindingPoint, const GPUSharedBufferData& gpuBufferData, bool isStatic)
{
	mGPUSharedBufferData = gpuBufferData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;
}

void GPUSharedBuffer::createBuffer()
{
    mBufferId = GET_SYSTEM(GPUInterface).createBuffer(mGPUSharedBufferData.mType);
    GET_SYSTEM(GPUInterface).bindSharedBufferToBindingPoint(mGPUSharedBufferData.mType, mBufferId, mBindingPoint);
    // mGPUPointer = glMapNamedBuffer(mBufferId, GL_READ_WRITE);

    for (size_t i = 0; i < GET_SYSTEM(GPUGlobalState).MAX_FRAMES_IN_FLIGHT; i++) {
        GPUAPI::GPUUniformBuffer uniformBuffer(GET_SYSTEM(GPUGlobalState).vulkanPhysicalDevice, GET_SYSTEM(GPUGlobalState).vulkanDevice);
        if (!uniformBuffer.initialize(sizeof(f32) * 200)) {
            CHECK_MSG(false, "Could not initialize uniform buffer for frame [{}]");
        }
        uniformBuffers.push_back(uniformBuffer);
    }
    VD_LOG_INFO("Initialized [{}] Vulkan uniform buffers", uniformBuffers.size());
}

void GPUSharedBuffer::terminate()
{
    // glUnmapNamedBuffer(mBufferId);
    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);

    for (GPUAPI::GPUUniformBuffer& uniformBuffer : uniformBuffers)
    {
        uniformBuffer.terminate();
    }
}

void GPUSharedBuffer::checkMaxSize(u32 bytes) const
{
    u32 maxBytes = GET_SYSTEM(GPUInterface).getMaxBytesInSharedBuffer(mGPUSharedBufferData.mType);
    CHECK_MSG(bytes <= maxBytes, "Max bytes reached in Shared Buffer!");
}
