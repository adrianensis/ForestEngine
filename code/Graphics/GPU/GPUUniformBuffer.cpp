#include "Graphics/GPU/GPUUniformBuffer.hpp"

GPUVariableData GPUUniformBufferData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = mInstanceName + "." + data.mName;
    return data;
}

void GPUUniformBuffer::init(u32 bindingPoint, const GPUUniformBufferData& gpuBlockData, bool isStatic)
{
	mGPUUniformBufferData = gpuBlockData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(GPUInterface).createUBO();

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUUniformBufferData.mGPUVariableDefinitionDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUUniformBufferData.mGPUVariableDefinitionDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }

	GET_SYSTEM(GPUInterface).bindUBO(mUBO, mBindingPoint);
}

void GPUUniformBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteUBO(mUBO);
}

void GPUUniformBuffer::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeUBOAnyType(mUBO, mSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}