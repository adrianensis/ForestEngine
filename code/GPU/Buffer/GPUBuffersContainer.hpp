#pragma once

#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Buffer/GPUIndicesBuffer.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"

class GPUVertexBuffersContainer
{
public:
	GPUVertexBuffersContainer() = default;

    void enable();
    void disable();
    void terminate();
    void addVertexBuffer(const GPUVertexBufferData& data, Core::u32 size, bool isStatic);
    GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data);
    const GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data) const;
    bool containsVertexBuffer(const GPUVertexBufferData& data) const;
    void setIndicesBuffer(const GPUDataType& gpuDataType, Core::u32 size, bool isStatic);

private:
    static Core::u32 findIndex(const std::unordered_map<Core::HashedString, Core::u32>& indexMap, const Core::HashedString& name);

private:
    Core::u32 mVertexBufferLayoutId = 0;
    GPUIndicesBuffer mIndicesBuffer;
    Core::u32 mAttributeIndex = 0;
    std::vector<GPUVertexBuffer> mVertexBuffers;
    std::unordered_map<Core::HashedString, Core::u32> mVertexBuffersMap;

public:
    CRGET(VertexBuffers)
    RGET(IndicesBuffer)
};

class GPUUniformBuffersContainer
{
public:
	GPUUniformBuffersContainer() = default;

    void terminate();
    void addUniformBuffer(const GPUUniformBufferData& data, Core::u32 size, bool isStatic);
    GPUUniformBuffer& getUniformBuffer(const GPUUniformBufferData& data);
    const GPUUniformBuffer& getUniformBuffer(const GPUUniformBufferData& data) const;
    GPUUniformBuffer& getUniformBuffer(Core::HashedString bufferName);
    const GPUUniformBuffer& getUniformBuffer(Core::HashedString bufferName) const;
    bool containsUniformBuffer(const GPUUniformBufferData& data) const;

private:
    static Core::u32 findIndex(const std::unordered_map<Core::HashedString, Core::u32>& indexMap, const Core::HashedString& name);

private:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::unordered_map<Core::HashedString, Core::u32> mUniformBuffersMap;
public:
    CRGET(UniformBuffers)
};
