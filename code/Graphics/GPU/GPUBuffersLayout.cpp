#include "Graphics/GPU/GPUBuffersLayout.hpp"

void GPUBuffersLayout::init(bool isStatic)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
    mVertexBufferLayoutId = GET_SYSTEM(GPUInterface).createVertexBufferLayout();
}

void GPUBuffersLayout::enable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUBuffersLayout::disable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}

u32 GPUBuffersLayout::createVertexBuffer(const GPUVertexBufferData& data)
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

void GPUBuffersLayout::setIndicesBuffer(const GPUDataType& gpuDataType)
{
    mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuDataType, mIsStatic);
}

GPUVertexBuffer& GPUBuffersLayout::getVertexBuffer(u32 index)
{
    return mVertexBuffers.at(index);
}

GPUVertexBuffer& GPUBuffersLayout::getVertexBuffer(const std::string& bufferName)
{
    CHECK_MSG(mVertexBuffersMap.contains(bufferName), bufferName + " not found in GPUBuffersLayout!");
    u32 index = mVertexBuffersMap.at(bufferName);
    return getVertexBuffer(index);
}

GPUVertexBuffer& GPUBuffersLayout::getVertexBuffer(const GPUVertexBufferData& data)
{
    return getVertexBuffer(data.mGPUVariableData.mName);
}

void GPUBuffersLayout::terminate()
{
    mIndicesBuffer.terminate();
    FOR_MAP(it, mVertexBuffers)
    {
        it->terminate();
    }

    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayoutId);
}