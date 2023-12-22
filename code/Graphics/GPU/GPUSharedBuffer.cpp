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

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUSharedBufferData.mGPUVariableDefinitionDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUSharedBufferData.mGPUVariableDefinitionDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }

    GET_SYSTEM(GPUInterface).bindSharedBufferToBindingPoint(mGPUSharedBufferData.mType, mBufferId, mBindingPoint);
}

void GPUSharedBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
}

void GPUSharedBuffer::resize(u32 size)
{
    GET_SYSTEM(GPUInterface).resizeBuffer(mGPUSharedBufferData.mType, mBufferId, mSizeInBytes, size, mIsStatic);
}