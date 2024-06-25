#include "Graphics/GPU/GPUIndicesBuffer.hpp"

void GPUIndicesBuffer::init(const GPUDataType& gpuDataType, bool isStatic)
{
    mIsStatic = isStatic;
    mGPUDataType = gpuDataType;
    mBufferId = GET_SYSTEM(GPUInterface).createBuffer();
}

void GPUIndicesBuffer::resize(u32 size)
{
    GET_SYSTEM(GPUInterface).resizeBuffer(GPUBufferType::INDEX, mBufferId, mGPUDataType.mTypeSizeInBytes, size, mIsStatic);
}

void GPUIndicesBuffer::terminate()
{
    GET_SYSTEM(GPUInterface).deleteBuffer(mBufferId);
}

