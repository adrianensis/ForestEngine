#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"

void GPUVertexBuffersContainer::create()
{
	PROFILER_CPU()

    mVertexBufferLayoutId = GET_SYSTEM(GPUInterface).createVertexBufferLayout();

    FOR_ARRAY(i, mVertexBuffers)
    {
        mVertexBuffers[i].createBuffer();
    }

    disable();
}

void GPUVertexBuffersContainer::enable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUVertexBuffersContainer::disable()
{
	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}

void GPUVertexBuffersContainer::addVertexBuffer(const GPUVertexBufferData& data, bool isStatic)
{
    if(mVertexBuffers.size() > 0)
    {
        mAttributeIndex = mVertexBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mVertexBuffers.emplace_back();
    gpuVertexBuffer.init(mAttributeIndex, data, isStatic);

    u32 index = mVertexBuffers.size() - 1;
    mVertexBuffersMap.insert_or_assign(data.mGPUVariableData.mName, index);
}

GPUVertexBuffer& GPUVertexBuffersContainer::getVertexBuffer(const GPUVertexBufferData& data)
{
    return mVertexBuffers.at(findIndex(mVertexBuffersMap, data.mGPUVariableData.mName));
}

const GPUVertexBuffer& GPUVertexBuffersContainer::getVertexBuffer(const GPUVertexBufferData& data) const
{
    return mVertexBuffers.at(findIndex(mVertexBuffersMap, data.mGPUVariableData.mName));
}

bool GPUVertexBuffersContainer::containsVertexBuffer(const GPUVertexBufferData& data) const
{
    return mVertexBuffersMap.contains(data.mGPUVariableData.mName);
}

u32 GPUVertexBuffersContainer::findIndex(const std::unordered_map<HashedString, u32>& indexMap, const HashedString& name)
{
    CHECK_MSG(indexMap.contains(name), name.get() + " not found in GPUVertexBuffersContainer!");
    u32 index = indexMap.at(name);
    return index;
}

void GPUVertexBuffersContainer::setIndicesBuffer(const GPUDataType& gpuDataType, bool isStatic)
{
    mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuDataType, isStatic);
}

void GPUVertexBuffersContainer::terminate()
{   
    mIndicesBuffer.terminate();
    FOR_LIST(it, mVertexBuffers)
    {
        it->terminate();
    }

    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUSharedBuffersContainer::create()
{   
    FOR_LIST(it, mSharedBuffers)
    {
        it->createBuffer();
    }
}

void GPUSharedBuffersContainer::addSharedBuffer(const GPUSharedBufferData& data, bool isStatic)
{
    u32 bindingPoint = GET_SYSTEM(GPUGlobalState).requestSharedBufferBindingPoint(data.mType);
    GPUSharedBuffer& gpuInstanceBuffer = mSharedBuffers.emplace_back();
    gpuInstanceBuffer.init(bindingPoint, data, isStatic);

    u32 index = mSharedBuffers.size() - 1;
    mSharedBuffersMap.insert_or_assign(data.mBufferName, index);
}

GPUSharedBuffer& GPUSharedBuffersContainer::getSharedBuffer(const GPUSharedBufferData& data)
{
    return mSharedBuffers.at(findIndex(mSharedBuffersMap, data.mBufferName));
}

const GPUSharedBuffer& GPUSharedBuffersContainer::getSharedBuffer(const GPUSharedBufferData& data) const
{
    return mSharedBuffers.at(findIndex(mSharedBuffersMap, data.mBufferName));
}

GPUSharedBuffer& GPUSharedBuffersContainer::getSharedBuffer(HashedString bufferName)
{
    return mSharedBuffers.at(findIndex(mSharedBuffersMap, bufferName));
}

const GPUSharedBuffer& GPUSharedBuffersContainer::getSharedBuffer(HashedString bufferName) const
{
    return mSharedBuffers.at(findIndex(mSharedBuffersMap, bufferName));
}

bool GPUSharedBuffersContainer::containsSharedBuffer(const GPUSharedBufferData& data) const
{
    return mSharedBuffersMap.contains(data.mBufferName);
}

u32 GPUSharedBuffersContainer::findIndex(const std::unordered_map<HashedString, u32>& indexMap, const HashedString& name)
{
    CHECK_MSG(indexMap.contains(name), name.get() + " not found in GPUSharedBuffersContainer!");
    u32 index = indexMap.at(name);
    return index;
}

void GPUSharedBuffersContainer::terminate()
{
    FOR_MAP(it, mSharedBuffers)
    {
        it->terminate();
    }
}