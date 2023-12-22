#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"

class GPUVertexBuffersLayout
{
public:
	GPUVertexBuffersLayout() = default;

    void init(bool isStatic);
    void enable();
    void disable();
    void terminate();
    u32 createBuffer(const GPUVertexBufferData& data);
    void setIndicesBuffer(const GPUDataType& gpuDataType);
    GPUVertexBuffer& getBuffer(u32 index);

private:
    u32 mVertexBufferLayoutId = 0;
    std::vector<GPUVertexBuffer> mBuffers;
    GPUIndicesBuffer mIndicesBuffer;
    u32 mAttributeIndex = 0;
	bool mIsStatic = false;
public:
    CRGET(Buffers)
    RGET(IndicesBuffer)
    CRGET(VertexBufferLayoutId)
};
