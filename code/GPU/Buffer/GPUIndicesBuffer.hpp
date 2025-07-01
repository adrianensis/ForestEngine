#pragma once

#include "GPU/Shader/GPUVariable.hpp"
#include "GPU/Buffer/GPUBuffer.h"

class GPUIndicesBuffer
{
public:
    void init(Core::Ptr<GPUContext> gpuContext, const GPUDataType& gpuDataType, Core::u32 size, bool isStatic);
    void resize(Core::u32 size);
    template <class T>
    void setDataArray(const std::vector<T>& data)
    {
        if (!setData((const void*)data.data(), data.size() * mGPUDataType.mTypeSizeInBytes))
        {
            CHECK_MSG(false, "Could not initialize Vulkan indices buffer");
        }
    }
    void setDataArray(const Core::ByteBuffer& data)
    {
        if (!setData((const void*)data.getBuffer().data(), data.getBuffer().size()))
        {
            CHECK_MSG(false, "Could not initialize Vulkan indices buffer");
        }
    }
    void terminate();

    const GPUBuffer& getGPUBuffer() const;
    bool setData(const void* data, Core::u32 size);

private:
    GPUDataType mGPUDataType;
    bool mIsStatic = false;

    
    Core::Ptr<GPUContext> mGPUContext;
    GPUBuffer mBuffer;
};