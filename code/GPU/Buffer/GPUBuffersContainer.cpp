#include "GPU/Buffer/GPUBuffersContainer.hpp"

void GPUVertexBuffersContainer::enable(GPUContext* gpuContext)
{
    const GPUCommandBuffer vulkanCommandBuffer = gpuContext->vulkanCommandBuffers[gpuContext->currentFrame];
    
    FOR_ARRAY(i, mVertexBuffers)
    {
        VkBuffer vertexBuffers[] = {mVertexBuffers[i].getGPUBuffer().getVkBuffer()};
        VkDeviceSize vertexBufferOffsets[] = {0};
        constexpr GPU::u32 bindingCount = 1;
        vkCmdBindVertexBuffers(vulkanCommandBuffer.getVkCommandBuffer(), i, bindingCount, vertexBuffers, vertexBufferOffsets);
    }

    constexpr VkDeviceSize indexBufferOffset = 0;
    constexpr VkIndexType indexType = VK_INDEX_TYPE_UINT32;
    vkCmdBindIndexBuffer(vulkanCommandBuffer.getVkCommandBuffer(), mIndicesBuffer.getGPUBuffer().getVkBuffer(), indexBufferOffset, indexType);
}

void GPUVertexBuffersContainer::disable()
{

}

void GPUVertexBuffersContainer::addVertexBuffer(GPUContext* gpuContext, const GPUVertexBufferData& data, GPU::u32 size, bool isStatic)
{
    if(mVertexBuffers.size() > 0)
    {
        mAttributeIndex = mVertexBuffers.back().getAttributeLocationWithOffset();
    }

    GPUVertexBuffer& gpuVertexBuffer = mVertexBuffers.emplace_back();
    gpuVertexBuffer.init(gpuContext, mAttributeIndex, data, size, isStatic);

    GPU::u32 index = mVertexBuffers.size() - 1;
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

GPU::u32 GPUVertexBuffersContainer::findIndex(const std::unordered_map<std::string, GPU::u32>& indexMap, const std::string& name)
{
    CHECK_MSG(indexMap.contains(name), name + " not found in GPUVertexBuffersContainer!");
    GPU::u32 index = indexMap.at(name);
    return index;
}

void GPUVertexBuffersContainer::setIndicesBuffer(GPUContext* gpuContext, const GPUDataType& gpuDataType, GPU::u32 size, bool isStatic)
{
    // mIndicesBuffer.terminate();
    mIndicesBuffer.init(gpuContext, gpuDataType, size, isStatic);
}

void GPUVertexBuffersContainer::terminate()
{   
    mIndicesBuffer.terminate();
    for(auto& it: mVertexBuffers)
    {
        it.terminate();
    }
}

void GPUUniformBuffersContainer::addUniformBuffer(GPUContext* gpuContext, const GPUUniformBufferData& data, GPU::u32 size, bool isStatic)
{
    GPU::u32 bindingPoint = gpuContext->requestUniformBufferBindingPoint(data.mType);
    GPUUniformBuffer& gpuInstanceBuffer = mUniformBuffers.emplace_back();
    gpuInstanceBuffer.init(gpuContext, size, bindingPoint, data, isStatic);

    GPU::u32 index = mUniformBuffers.size() - 1;
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

GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(std::string bufferName)
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, bufferName));
}

const GPUUniformBuffer& GPUUniformBuffersContainer::getUniformBuffer(std::string bufferName) const
{
    return mUniformBuffers.at(findIndex(mUniformBuffersMap, bufferName));
}

bool GPUUniformBuffersContainer::containsUniformBuffer(const GPUUniformBufferData& data) const
{
    return mUniformBuffersMap.contains(data.mBufferName);
}

GPU::u32 GPUUniformBuffersContainer::findIndex(const std::unordered_map<std::string, GPU::u32>& indexMap, const std::string& name)
{
    CHECK_MSG(indexMap.contains(name), name + " not found in GPUUniformBuffersContainer!");
    GPU::u32 index = indexMap.at(name);
    return index;
}

void GPUUniformBuffersContainer::terminate()
{
    FOR_MAP(it, mUniformBuffers)
    {
        it->terminate();
    }
}