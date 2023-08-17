#include "Graphics/GPU/GPUSharedBuffer.hpp"

void GPUSharedBuffer::init(const GPUSharedBufferData& data, bool isStatic)
{
	mData = data;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(RenderContext).createUBO();
}

void GPUSharedBuffer::terminate()
{
    GET_SYSTEM(RenderContext).deleteUBO(mUBO);
}

void GPUSharedBuffer::resize(u32 size)
{
	GET_SYSTEM(RenderContext).resizeUBOAnyType(mUBO, mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, size, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}