#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

class GPUBuffersLayout
{
public:
	GPUBuffersLayout() = default;

    void init(bool isStatic);
    void enable();
    void disable();
    void terminate();
    void createVertexBuffer(const GPUVertexBufferData& data);
    GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data);
    const GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data) const;
    void createSharedBuffer(const GPUSharedBufferData& data);
    GPUSharedBuffer& getSharedBuffer(const GPUSharedBufferData& data);
    const GPUSharedBuffer& getSharedBuffer(const GPUSharedBufferData& data) const;
    void setIndicesBuffer(const GPUDataType& gpuDataType);

private:
    u32 findIndex(const std::unordered_map<std::string, u32>& indexMap, const std::string& name) const;

private:
    u32 mVertexBufferLayoutId = 0;
    GPUIndicesBuffer mIndicesBuffer;
    u32 mAttributeIndex = 0;
    std::vector<GPUVertexBuffer> mVertexBuffers;
    std::unordered_map<std::string, u32> mVertexBuffersMap;
    std::vector<GPUSharedBuffer> mInstanceBuffers;
    std::unordered_map<std::string, u32> mInstanceBuffersMap;
	bool mIsStatic = false;
public:
    CRGET(VertexBuffers)
    RGET(IndicesBuffer)
};
