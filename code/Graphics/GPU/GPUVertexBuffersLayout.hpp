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
    GPUVertexBuffer& getBuffer(const std::string& bufferName);
    GPUVertexBuffer& getBuffer(const GPUVertexBufferData& data);

private:
    u32 mVertexBufferLayoutId = 0;
    std::vector<GPUVertexBuffer> mBuffers;
    std::unordered_map<std::string, u32> mBuffersMap;
    GPUIndicesBuffer mIndicesBuffer;
    u32 mAttributeIndex = 0;
	bool mIsStatic = false;
public:
    CRGET(Buffers)
    CRGET(BuffersMap)
    RGET(IndicesBuffer)
    CRGET(VertexBufferLayoutId)
};
