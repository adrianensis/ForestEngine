#include "Graphics/GPU/GPUSharedBlock.hpp"

void GPUSharedBlock::init(u32 bindingPoint, const GPUSharedBlockData& gpuBlockData, bool isStatic)
{
	mGPUSharedBlockData = gpuBlockData;
    mIsStatic = isStatic;
	mUBO = GET_SYSTEM(GPUInterface).createUBO();
    mBindingPoint = bindingPoint;

    mSizeInBytes = 0;
    FOR_ARRAY(i, mGPUSharedBlockData.mGPUVariableDataArray)
    {
        const GPUVariableData& gpuVariableData = mGPUSharedBlockData.mGPUVariableDataArray[i];
        mSizeInBytes += gpuVariableData.mGPUDataType.mTypeSizeInBytes;
    }

	GET_SYSTEM(GPUInterface).bindUBO(mUBO, mBindingPoint);
    resize(mSizeInBytes);
}

void GPUSharedBlock::terminate()
{
    GET_SYSTEM(GPUInterface).deleteUBO(mUBO);
}

void GPUSharedBlock::resize(u32 size)
{
	GET_SYSTEM(GPUInterface).resizeUBOAnyType(mUBO, mSizeInBytes, 1, mIsStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}