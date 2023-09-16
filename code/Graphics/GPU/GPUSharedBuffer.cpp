#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

void GPUSharedBuffer::init(const GPUSharedBufferData& data, bool isStatic)
{
	mData = data;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(GPUInterface).createUBO();
}

void GPUSharedBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteUBO(mUBO);
}

void GPUSharedBuffer::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeUBOAnyType(mUBO, mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}