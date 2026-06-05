#include "GPU/Buffer/GPUUniformBuffer.hpp"
#include "Core/CoreMacros.hpp"
#include "GPU/Core/GPUContext.hpp"

GPUVariableData GPUUniformBufferData::getScopedGPUVariableData(GPU::u32 i) const
{
    GPUVariableData data = mGPUVariableDefinitionDataArray[i];
    data.mName = std::string(mInstanceName + "." + data.mName);
    return data;
}

void GPUUniformBuffer::init(GPUContext* gpuContext, GPU::u32 size, GPU::u32 bindingPoint, const GPUUniformBufferData& gpuUniformBufferData, bool isStatic)
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

    FOR_RANGE(i, 0, GPUContext::MAX_FRAMES_IN_FLIGHT)
    {
        if (!mBuffers[i].init(mGPUContext, gpuBufferData)) {
            CHECK_MSG(false,"Could not initialize uniform buffer");
        }
    }

    // LOG("Initialized uniform buffer");
}

void GPUUniformBuffer::resize(GPU::u32 size)
{
    mSize = size;
    FOR_RANGE(i, 0, GPUContext::MAX_FRAMES_IN_FLIGHT)
    {
        mBuffers[i].resize(size);
    }
}

void GPUUniformBuffer::terminate() 
{
    FOR_RANGE(i, 0, GPUContext::MAX_FRAMES_IN_FLIGHT)
    {
        mBuffers[i].terminate();
    }
}