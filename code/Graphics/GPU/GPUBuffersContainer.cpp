#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"
#include "Graphics/GPU/GPUInstance.hpp"

void GPUVertexBuffersContainer::create()
{
	PROFILER_CPU()

//    mVertexBufferLayoutId = GET_SYSTEM(GPUInterface).createVertexBufferLayout();

    FOR_ARRAY(i, mVertexBuffers)
    {
        mVertexBuffers[i].createBuffer();
    }

    disable();
}

void GPUVertexBuffersContainer::enable()
{
//	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUVertexBuffersContainer::disable()
{
//	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(0);
}

void GPUVertexBuffersContainer::addVertexBuffer(const GPUVertexBufferData& data, bool isStatic)
{
    if(mVertexBuffers.size() > 0)
    {
        mAttributeIndex = mVertexBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mVertexBuffers.emplace_back();
    // gpuVertexBuffer.init(mAttributeIndex, data, isStatic);

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
    // mIndicesBuffer.terminate();
    // mIndicesBuffer.init(gpuDataType, isStatic);
}

void GPUVertexBuffersContainer::terminate()
{   
    mIndicesBuffer.terminate();
    FOR_LIST(it, mVertexBuffers)
    {
        it->terminate();
    }

//    GET_SYSTEM(GPUInterface).deleteVertexBufferLayout(mVertexBufferLayoutId);
}

void GPUUniformBuffersContainer::create()
{   
    FOR_LIST(it, mUniformBuffers)
    {
        it->createBuffer();
    }
}

void GPUUniformBuffersContainer::addUniformBuffer(const GPUUniformBufferData& data, bool isStatic)
{
    u32 bindingPoint = GET_SYSTEM(GPUInstance).requestUniformBufferBindingPoint(data.mType);
    GPUUniformBuffer& gpuInstanceBuffer = mUniformBuffers.emplace_back();
    // gpuInstanceBuffer.init(bindingPoint, data, isStatic);

    u32 index = mUniformBuffers.size() - 1;
    mUniformBuffersMap.insert_or_assign(data.mBufferName, index);
}

GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(const GPUUniformBufferData& data)
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, data.mBufferName));
}

const GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(const GPUUniformBufferData& data) const
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, data.mBufferName));
}

GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(HashedString bufferName)
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, bufferName));
}

const GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(HashedString bufferName) const
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, bufferName));
}

bool GPUUniformBuffersContainer::containsUniformBuffer(const GPUUniformBufferData& data) const
{
    return mUniformBuffersMap.contains(data.mBufferName);
}

u32 GPUUniformBuffersContainer::findIndex(const std::unordered_map<HashedString, u32>& indexMap, const HashedString& name)
{
    CHECK_MSG(indexMap.contains(name), name.get() + " not found in GPUUniformBuffersContainer!");
    u32 index = indexMap.at(name);
    return index;
}

void GPUUniformBuffersContainer::terminate()
{
    FOR_MAP(it, mUniformBuffers)
    {
        it->terminate();
    }
}