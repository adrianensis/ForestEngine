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

u32 GPUVertexBuffersLayout::createVertexBuffer(const GPUVertexBufferData& data)
{
    if(mVertexBuffers.size() > 0)
    {
        mAttributeIndex = mVertexBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mVertexBuffers.emplace_back();
    gpuVertexBuffer.init(mAttributeIndex, data, mIsStatic);

    u32 index = mVertexBuffers.size() - 1;
    mVertexBuffersMap.insert_or_assign(data.mGPUVariableData.mName, index);

    return index;
}

void GPUVertexBuffersLayout::setIndicesBuffer(const GPUDataType& gpuDataType)
{
    mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuDataType, mIsStatic);
}

GPUVertexBuffer& GPUVertexBuffersLayout::getVertexBuffer(u32 index)
{
    return mVertexBuffers.at(index);
}

GPUVertexBuffer& GPUVertexBuffersLayout::getVertexBuffer(const std::string& bufferName)
{
    CHECK_MSG(mVertexBuffersMap.contains(bufferName), bufferName + " not found in GPUVertexBuffersLayout!");
    u32 index = mVertexBuffersMap.at(bufferName);
    return getVertexBuffer(index);
}

GPUVertexBuffer& GPUVertexBuffersLayout::getVertexBuffer(const GPUVertexBufferData& data)
{
    return getVertexBuffer(data.mGPUVariableData.mName);
}

void GPUVertexBuffersLayout::terminate()
{
    mIndicesBuffer.terminate();
    FOR_MAP(it, mVertexBuffers)
    {
        it->terminate();
    }

    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayoutId);
}