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
    u32 createVertexBuffer(const GPUVertexBufferData& data);
    GPUVertexBuffer& getVertexBuffer(u32 index);
    GPUVertexBuffer& getVertexBuffer(const std::string& bufferName);
    GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data);
    void setIndicesBuffer(const GPUDataType& gpuDataType);

private:
    u32 mVertexBufferLayoutId = 0;
    std::vector<GPUVertexBuffer> mVertexBuffers;
    std::unordered_map<std::string, u32> mVertexBuffersMap;
    GPUIndicesBuffer mIndicesBuffer;
    u32 mAttributeIndex = 0;
	bool mIsStatic = false;
public:
    CRGET(VertexBuffers)
    CRGET(VertexBuffersMap)
    RGET(IndicesBuffer)
    CRGET(VertexBufferLayoutId)
};
