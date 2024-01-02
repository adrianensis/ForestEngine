#include "Graphics/GPU/GPUSharedBuffer.hpp"

GPUVariableData GPUSharedBufferData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = mInstanceName + "." + data.mName;
    return data;
}

void GPUSharedBuffer::init(u32 bindingPoint, const GPUSharedBufferData& gpuBufferData, bool isStatic)
{
	mGPUSharedBufferData = gpuBufferData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;

    mBufferId = GET_SYSTEM(GPUInterface).createBuffer();

    GET_SYSTEM(GPUInterface).bindSharedBufferToBindingPoint(mGPUSharedBufferData.mType, mBufferId, mBindingPoint);
}

void GPUSharedBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
}

void GPUSharedBuffer::checkMaxSize(u32 bytes) const
{
    u32 maxBytes = GET_SYSTEM(GPUInterface).getMaxBytesInSharedBuffer(mGPUSharedBufferData.mType);
    CHECK_MSG(bytes <= maxBytes, "Max bytes reached in Shared Buffer!");
}
