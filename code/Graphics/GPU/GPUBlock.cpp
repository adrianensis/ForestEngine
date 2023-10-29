#include "Graphics/GPU/GPUBlock.hpp"

void GPUBlock::init(const GPUBlockData& gpuBlockData, bool isStatic)
{
	mGPUBlockData = gpuBlockData;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(GPUInterface).createUBO();

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUBlockData.mGPUVariableDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUBlockData.mGPUVariableDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }
}

void GPUBlock::terminate()
{
    GET_SYSTEM(GPUInterface).deleteUBO(mUBO);
}

void GPUBlock::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeUBOAnyType(mUBO, mSizeInBytes, 1, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}