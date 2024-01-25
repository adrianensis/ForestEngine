#include "Graphics/GPU/GPUBuffersLayout.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

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

void GPUBuffersLayout::createVertexBuffer(const GPUVertexBufferData& data)
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

GPUVertexBuffer& GPUBuffersLayout::getVertexBuffer(const GPUVertexBufferData& data)
{
    return mVertexBuffers.at(findIndex(mVertexBuffersMap, data.mGPUVariableData.mName));
}

const GPUVertexBuffer& GPUBuffersLayout::getVertexBuffer(const GPUVertexBufferData& data) const
{
    return mVertexBuffers.at(findIndex(mVertexBuffersMap, data.mGPUVariableData.mName));
}

void GPUBuffersLayout::createInstanceBuffer(const GPUSharedBufferData& data)
{
    u32 bindingPoint = GET_SYSTEM(GPUSharedContext).requestSharedBufferBindingPoint(data.mType);
    GPUSharedBuffer& gpuInstanceBuffer = mInstanceBuffers.emplace_back();
    gpuInstanceBuffer.init(bindingPoint, data, mIsStatic);

    u32 index = mInstanceBuffers.size() - 1;
    mInstanceBuffersMap.insert_or_assign(data.mBufferName, index);
}

GPUSharedBuffer& GPUBuffersLayout::getInstanceBuffer(const GPUSharedBufferData& data)
{
    return mInstanceBuffers.at(findIndex(mInstanceBuffersMap, data.mBufferName));
}

const GPUSharedBuffer& GPUBuffersLayout::getInstanceBuffer(const GPUSharedBufferData& data) const
{
    return mInstanceBuffers.at(findIndex(mInstanceBuffersMap, data.mBufferName));
}

u32 GPUBuffersLayout::findIndex(const std::unordered_map<std::string, u32>& indexMap, const std::string& name) const
{
    CHECK_MSG(indexMap.contains(name), name + " not found in GPUBuffersLayout!");
    u32 index = indexMap.at(name);
    return index;
}

void GPUBuffersLayout::setIndicesBuffer(const GPUDataType& gpuDataType)
{
    mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuDataType, mIsStatic);
}

void GPUBuffersLayout::terminate()
{
    FOR_MAP(it, mInstanceBuffers)
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