#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUBuffer.hpp"

class GPUIndicesBuffer
{
public:
	GPUIndicesBuffer() = default;

    void init(const GPUDataType& gpuDataType, bool isStatic);
    void resize(u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
//	    GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mBufferId, data);
    }
    void setDataArray(const ByteBuffer& data)
    {
//	    GET_SYSTEM(GPUInterface).setBufferDataArray(GPUBufferType::INDEX, mBufferId, data);
    }
    void terminate();

    GPUIndexBuffer(GPUPhysicalDevice* vulkanPhysicalDevice, GPUDevice* vulkanDevice, GPUCommandPool* vulkanCommandPool);
    const GPUBuffer& getGPUBuffer() const;
    bool initialize(const std::vector<uint32_t>& indices);

private:
	u32 mBufferId = 0;
    GPUDataType mGPUDataType;
    bool mIsStatic = false;

    GPUPhysicalDevice* vulkanPhysicalDevice;
    GPUDevice* vulkanDevice;
    GPUCommandPool* vulkanCommandPool;
    GPUBuffer buffer;

public:
    GET(BufferId)
};