#include "Graphics/Buffers/GPUBuffersLayout.hpp"

GPUBuffersLayout::~GPUBuffersLayout() 
{
    terminate();
}

void GPUBuffersLayout::init(bool isStatic)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
}

u32 GPUBuffersLayout::addBuffer(const GPUBufferData& data)
{
    if(mBuffers.size() > 0)
    {
        mAttributeIndex = mBuffers.back().getAttributeLocationWithOffset();
    }

    GPUBuffer& gpuBuffer = mBuffers.emplace_back();
    gpuBuffer.init(mAttributeIndex, data, mIsStatic);

    return mBuffers.size() - 1;
}

GPUBuffer& GPUBuffersLayout::getBuffer(u32 index)
{
    return mBuffers.at(index);
}

void GPUBuffersLayout::terminate()
{
    FOR_MAP(it, mBuffers)
    {
        it->terminate();
    }
}