#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

void GPUBuffersContainer::init(bool isStatic)
{
	PROFILER_CPU()

	mIsStatic = isStatic;
    mVertexBufferLayoutId = GET_SYSTEM(GPUInterface).createVertexBufferLayout();
}

void GPUBuffersContainer::enable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUBuffersContainer::disable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}

void GPUBuffersContainer::createVertexBuffer(const GPUVertexBufferData& data)
{
    if(mVertexBuffers.size() > 0)
    {
        mAttributeIndex = mVertexBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mVertexBuffers.emplace_back();
    gpuVertexBuffer.init(mAttributeIndex, data, mIsStatic);

    u32 index = mVertexBuffers.size() - 1;
    mVertexBuffersMap.insert_or_assign(data.mGPUVariableData.mName, index);
}

GPUVertexBuffer& GPUBuffersContainer::getVertexBuffer(const GPUVertexBufferData& data)
{
    return mVertexBuffers.at(findIndex(mVertexBuffersMap, data.mGPUVariableData.mName));
}

const GPUVertexBuffer& GPUBuffersContainer::getVertexBuffer(const GPUVertexBufferData& data) const
{
    return mVertexBuffers.at(findIndex(mVertexBuffersMap, data.mGPUVariableData.mName));
}

void GPUBuffersContainer::createSharedBuffer(const GPUSharedBufferData& data)
{
    u32 bindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(data.mType);
    GPUSharedBuffer& gpuInstanceBuffer = mSharedBuffers.emplace_back();
    gpuInstanceBuffer.init(bindingPoint, data, mIsStatic);

    u32 index = mSharedBuffers.size() - 1;
    mSharedBuffersMap.insert_or_assign(data.mBufferName, index);
}

GPUSharedBuffer& GPUBuffersContainer::getSharedBuffer(const GPUSharedBufferData& data)
{
    return mSharedBuffers.at(findIndex(mSharedBuffersMap, data.mBufferName));
}

const GPUSharedBuffer& GPUBuffersContainer::getSharedBuffer(const GPUSharedBufferData& data) const
{
    return mSharedBuffers.at(findIndex(mSharedBuffersMap, data.mBufferName));
}

u32 GPUBuffersContainer::findIndex(const std::unordered_map<std::string, u32>& indexMap, const std::string& name) const
{
    CHECK_MSG(indexMap.contains(name), name + " not found in GPUBuffersContainer!");
    u32 index = indexMap.at(name);
    return index;
}

void GPUBuffersContainer::setIndicesBuffer(const GPUDataType& gpuDataType)
{
    mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuDataType, mIsStatic);
}

void GPUBuffersContainer::terminate()
{
    FOR_MAP(it, mSharedBuffers)
    {
        it->terminate();
    }
    
    mIndicesBuffer.terminate();
    FOR_MAP(it, mVertexBuffers)
    {
        it->terminate();
    }

    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayoutId);
}