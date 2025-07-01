#include "GPU/Buffer/GPUBuffersContainer.hpp"
#include "GPU/GPUInstance.hpp"

void GPUVertexBuffersContainer::enable()
{
//	GET_SYSTEM(GPUInterface).enableVertexBufferLayout(mVertexBufferLayoutId);
    const GPUCommandBuffer vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    
    FOR_ARRAY(i, mVertexBuffers)
    {
        VkBuffer vertexBuffers[] = {mVertexBuffers[i].getGPUBuffer().getVkBuffer()};
        VkDeviceSize vertexBufferOffsets[] = {0};
        constexpr Core::u32 bindingCount = 1;
        vkCmdBindVertexBuffers(vulkanCommandBuffer.getVkCommandBuffer(), i, bindingCount, vertexBuffers, vertexBufferOffsets);
    }

    constexpr VkDeviceSize indexBufferOffset = 0;
    constexpr VkIndexType indexType = VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(vulkanCommandBuffer.getVkCommandBuffer(), mIndicesBuffer.getGPUBuffer().getVkBuffer(), indexBufferOffset, indexType);
}

void GPUVertexBuffersContainer::disable()
{

}

void GPUVertexBuffersContainer::addVertexBuffer(const GPUVertexBufferData& data, Core::u32 size, bool isStatic)
{
    if(mVertexBuffers.size() > 0)
    {
        mAttributeIndex = mVertexBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mVertexBuffers.emplace_back();
    gpuVertexBuffer.init(GET_SYSTEM(GPUInstance).mGPUContext, mAttributeIndex, data, size, isStatic);

    Core::u32 index = mVertexBuffers.size() - 1;
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

Core::u32 GPUVertexBuffersContainer::findIndex(const std::unordered_map<Core::HashedString, Core::u32>& indexMap, const Core::HashedString& name)
{
    CHECK_MSG(indexMap.contains(name), name.get() + " not found in GPUVertexBuffersContainer!");
    Core::u32 index = indexMap.at(name);
    return index;
}

void GPUVertexBuffersContainer::setIndicesBuffer(const GPUDataType& gpuDataType, Core::u32 size, bool isStatic)
{
    // mIndicesBuffer.terminate();
    mIndicesBuffer.init(GET_SYSTEM(GPUInstance).mGPUContext, gpuDataType, size, isStatic);
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

void GPUUniformBuffersContainer::addUniformBuffer(const GPUUniformBufferData& data, Core::u32 size, bool isStatic)
{
    Core::u32 bindingPoint = GET_SYSTEM(GPUInstance).requestUniformBufferBindingPoint(data.mType);
    GPUUniformBuffer& gpuInstanceBuffer = mUniformBuffers.emplace_back();
    gpuInstanceBuffer.init(GET_SYSTEM(GPUInstance).mGPUContext, size, bindingPoint, data, isStatic);

    Core::u32 index = mUniformBuffers.size() - 1;
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

GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(Core::HashedString bufferName)
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, bufferName));
}

const GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(Core::HashedString bufferName) const
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, bufferName));
}

bool GPUUniformBuffersContainer::containsUniformBuffer(const GPUUniformBufferData& data) const
{
    return mUniformBuffersMap.contains(data.mBufferName);
}

Core::u32 GPUUniformBuffersContainer::findIndex(const std::unordered_map<Core::HashedString, Core::u32>& indexMap, const Core::HashedString& name)
{
    CHECK_MSG(indexMap.contains(name), name.get() + " not found in GPUUniformBuffersContainer!");
    Core::u32 index = indexMap.at(name);
    return index;
}

void GPUUniformBuffersContainer::terminate()
{
    FOR_MAP(it, mUniformBuffers)
    {
        it->terminate();
    }
}