#pragma once

#include "Graphics/GPU/GPUVariable.hpp"
#include "Graphics/GPU/GPUInterface.hpp"

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

private:
	u32 mBufferId = 0;
    GPUDataType mGPUDataType;
    bool mIsStatic = false;
public:
    GET(BufferId)
};