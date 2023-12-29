#include "Graphics/GPU/GPUVertexBuffersLayout.hpp"

void GPUVertexBuffersLayout::init(bool isStatic)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
    mVertexBufferLayoutId = GET_SYSTEM(GPUInterface).createVertexBufferLayout();
}

void GPUVertexBuffersLayout::enable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUVertexBuffersLayout::disable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}

u32 GPUVertexBuffersLayout::createBuffer(const GPUVertexBufferData& data)
{
    if(mBuffers.size() > 0)
    {
        mAttributeIndex = mBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mBuffers.emplace_back();
    gpuVertexBuffer.init(mAttributeIndex, data, mIsStatic);

    u32 index = mBuffers.size() - 1;
    mBuffersMap.insert_or_assign(data.mGPUVariableData.mName, index);

    return index;
}

void GPUVertexBuffersLayout::setIndicesBuffer(const GPUDataType& gpuDataType)
{
    mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuDataType, mIsStatic);
}

GPUVertexBuffer& GPUVertexBuffersLayout::getBuffer(u32 index)
{
    return mBuffers.at(index);
}

GPUVertexBuffer& GPUVertexBuffersLayout::getBuffer(const std::string& bufferName)
{
    CHECK_MSG(mBuffersMap.contains(bufferName), bufferName + " not found in GPUVertexBuffersLayout!");
    u32 index = mBuffersMap.at(bufferName);
    return getBuffer(index);
}

GPUVertexBuffer& GPUVertexBuffersLayout::getBuffer(const GPUVertexBufferData& data)
{
    return getBuffer(data.mGPUVariableData.mName);
}

void GPUVertexBuffersLayout::terminate()
{
    mIndicesBuffer.terminate();
    FOR_MAP(it, mBuffers)
    {
        it->terminate();
    }

    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayoutId);
}