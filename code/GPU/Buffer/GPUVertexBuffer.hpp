#pragma once

#include "GPU/Shader/GPUVariable.hpp"

#include "GPU/Core/GPUPhysicalDevice.h"
#include "GPU/Core/GPUDevice.h"
#include "GPU/Core/GPUCommandBuffer.h"
#include "GPU/Buffer/GPUBuffer.h"
#include "Core/Memory/ByteBuffer.hpp"

class GPUVertexBufferData
{
public:
	GPUVertexBufferData() = default;
    GPUVertexBufferData(const GPUVariableData& gpuVariableData): mGPUVariableData(gpuVariableData) {};
    GPUVertexBufferData(const GPUVariableData& gpuVariableData, GPU::u32 instanceDivisor): mGPUVariableData(gpuVariableData), mInstanceDivisor(instanceDivisor) {};
    
public:
    GPUVariableData mGPUVariableData;
    GPU::u32 mInstanceDivisor = 0;
};

class GPUVertexBuffer
{
public:
    void init(GPUContext* gpuContext, GPU::u32 attributeLocation, const GPUVertexBufferData& data, GPU::u32 size, bool isStatic);
    void resize(GPU::u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data, VkCommandBuffer commandBuffer)
    {
        if (!setData((const void*)data.data(), data.size() * mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, &commandBuffer))
        {
            CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
    }
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        if (!setData((const void*)data.data(), data.size() * mData.mGPUVariableData.mGPUDataType.mTypeSizeInBytes, nullptr))
        {
            CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
    }
    void setDataArray(const Core::ByteBuffer& data)
    {
        if (!setData((const void*)data.getBuffer().data(), data.getBuffer().size(), nullptr))
        {
            CHECK_MSG(false, "Could not initialize Vulkan vertex buffer");
        }
    }
    GPU::u32 getAttributeLocation() const;
    GPU::u32 getAttributeLocationWithOffset() const;
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool setData(const void* data, GPU::u32 size, VkCommandBuffer* commandBuffer);

public:
    GPUVertexBufferData mData;
private:
    GPU::u32 mAttributeLocation = 0;
    GPU::u32 mAttributeOffset = 0;
    GPU::u32 mPreviousOffsetInBytes = 0;
    bool mIsStatic = false;

    GPUContext* mGPUContext = nullptr;
    GPUBuffer mBuffer;
    GPUBuffer stagingBuffer;
};