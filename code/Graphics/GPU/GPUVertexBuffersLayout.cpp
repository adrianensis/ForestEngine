#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"

GPUVertexBuffersLayout::~GPUVertexBuffersLayout() 
{
    terminate();
}

void GPUVertexBuffersLayout::init(bool isStatic)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
}

u32 GPUVertexBuffersLayout::addBuffer(const GPUVertexBufferData& data)
{
    if(mBuffers.size() > 0)
    {
        mAttributeIndex = mBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mBuffers.emplace_back();
    gpuVertexBuffer.init(mAttributeIndex, data, mIsStatic);

    return mBuffers.size() - 1;
}

GPUVertexBuffer& GPUVertexBuffersLayout::getBuffer(u32 index)
{
    return mBuffers.at(index);
}

void GPUVertexBuffersLayout::terminate()
{
    FOR_MAP(it, mBuffers)
    {
        it->terminate();
    }
}