#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUIndicesBuffer.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"

class GPUVertexBuffersContainer
{
public:
	GPUVertexBuffersContainer() = default;

    void create();
    void enable();
    void disable();
    void terminate();
    void addVertexBuffer(const GPUVertexBufferData& data, bool isStatic);
    GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data);
    const GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data) const;
    bool containsVertexBuffer(const GPUVertexBufferData& data) const;
    void setIndicesBuffer(const GPUDataType& gpuDataType, bool isStatic);

private:
    static u32 findIndex(const std::unordered_map<HashedString, u32>& indexMap, const HashedString& name);

private:
    u32 mVertexBufferLayoutId = 0;
    GPUIndicesBuffer mIndicesBuffer;
    u32 mAttributeIndex = 0;
    std::vector<GPUVertexBuffer> mVertexBuffers;
    std::unordered_map<HashedString, u32> mVertexBuffersMap;
public:
    CRGET(VertexBuffers)
    RGET(IndicesBuffer)
};

class GPUSharedBuffersContainer
{
public:
	GPUSharedBuffersContainer() = default;

    void terminate();
    void create();
    void addSharedBuffer(const GPUSharedBufferData& data, bool isStatic);
    GPUSharedBuffer& getSharedBuffer(const GPUSharedBufferData& data);
    const GPUSharedBuffer& getSharedBuffer(const GPUSharedBufferData& data) const;
    GPUSharedBuffer& getSharedBuffer(HashedString bufferName);
    const GPUSharedBuffer& getSharedBuffer(HashedString bufferName) const;
    bool containsSharedBuffer(const GPUSharedBufferData& data) const;

private:
    static u32 findIndex(const std::unordered_map<HashedString, u32>& indexMap, const HashedString& name);

private:
    std::vector<GPUSharedBuffer> mSharedBuffers;
    std::unordered_map<HashedString, u32> mSharedBuffersMap;
public:
    CRGET(SharedBuffers)
};
