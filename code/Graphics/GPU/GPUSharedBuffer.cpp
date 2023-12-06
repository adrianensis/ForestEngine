#include "Graphics/GPU/GPUSharedBuffer.hpp"

GPUVariableData GPUSharedBufferData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = mInstanceName + "." + data.mName;
    return data;
}

void GPUSharedBuffer::init(u32 bindingPoint, const GPUSharedBufferData& gpuBlockData, bool isStatic)
{
	mGPUSharedBufferData = gpuBlockData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;

    switch (mGPUSharedBufferData.mType)
    {
    case UNIFORM:
        mBufferId = GET_SYSTEM(GPUInterface).createUBO();
        break;
    case STORAGE:
        mBufferId = GET_SYSTEM(GPUInterface).createSSBO();
        break;
    }

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUSharedBufferData.mGPUVariableDefinitionDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUSharedBufferData.mGPUVariableDefinitionDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }

    switch (mGPUSharedBufferData.mType)
    {
    case UNIFORM:
        GET_SYSTEM(GPUInterface).bindUBO(mBufferId, mBindingPoint);
        break;
    case STORAGE:
        GET_SYSTEM(GPUInterface).bindSSBO(mBufferId, mBindingPoint);
        break;
    }
}

void GPUSharedBuffer::terminate()
{
    switch (mGPUSharedBufferData.mType)
    {
    case UNIFORM:
        GET_SYSTEM(GPUInterface).deleteUBO(mBufferId);
        break;
    case STORAGE:
        GET_SYSTEM(GPUInterface).deleteSSBO(mBufferId);
        break;
    }
}

void GPUSharedBuffer::resize(u32 size)
{
    switch (mGPUSharedBufferData.mType)
    {
    case UNIFORM:
	    GET_SYSTEM(GPUInterface).resizeUBOAnyType(mBufferId, mSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        break;
    case STORAGE:
	    GET_SYSTEM(GPUInterface).resizeSSBOAnyType(mBufferId, mSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        break;
    }
}