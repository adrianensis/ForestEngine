#include "GPU/Buffer/GPUUniformBuffer.hpp"

GPUVariableData GPUUniformBufferData::getScopedGPUVariableData(u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = HashedString(mInstanceName.get() + "." + data.mName.get());
    return data;
}

void GPUUniformBuffer::init(WeakPtr<GPUContext> gpuContext, u32 size, u32 bindingPoint, const GPUUniformBufferData& gpuUniformBufferData, bool isStatic)
{
    mGPUContext = gpuContext;
	mGPUUniformBufferData = gpuUniformBufferData;
    mBindingPoint = bindingPoint;
    mIsStatic = isStatic;
    mSize = size;

    GPUBufferData gpuBufferData{};
    gpuBufferData.Size = mSize;
    switch (mGPUUniformBufferData.mType)
    {
    case GPUBufferType::UNIFORM:
        gpuBufferData.Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        break;
    case GPUBufferType::STORAGE:
        gpuBufferData.Usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        break;
    }
    gpuBufferData.MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (!mBuffer.init(mGPUContext, gpuBufferData)) {
        CHECK_MSG(false,"Could not initialize uniform buffer");
    }

    LOG("Initialized uniform buffer");
}

void GPUUniformBuffer::resize(u32 size)
{
    mBuffer.resize(size);
}

void GPUUniformBuffer::terminate() {
    mBuffer.terminate();
}

void GPUUniformBuffer::setData(const void* data) const {
    mBuffer.setData(data);
}