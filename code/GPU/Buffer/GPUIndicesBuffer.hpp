#pragma once

#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"
#include "GPU/Core/GPUByteBuffer.hpp"

class GPUIndicesBuffer
{
public:
    void init(GPUContext* gpuContext, const GPUDataType& gpuDataType, GPU::u32 size, bool isStatic);
    void resize(GPU::u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        if (!setData((const void*)data.data(), data.size() * mGPUDataType.mTypeSizeInBytes))
        {
            CHECK_MSG(false, "Could not initialize Vulkan indices buffer");
        }
    }
    void setDataArray(const GPU::ByteBuffer& data)
    {
        if (!setData((const void*)data.getBuffer().data(), data.getBuffer().size()))
        {
            CHECK_MSG(false, "Could not initialize Vulkan indices buffer");
        }
    }
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool setData(const void* data, GPU::u32 size);

private:
    GPUDataType mGPUDataType;
    bool mIsStatic = false;

    
    GPUContext* mGPUContext = nullptr;
    GPUBuffer mBuffer;
};