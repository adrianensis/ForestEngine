#pragma once

#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Buffer/GPUIndicesBuffer.hpp"
#include "GPU/Buffer/GPUUniformBuffer.hpp"

class GPUVertexBuffersContainer
{
public:
	GPUVertexBuffersContainer() = default;

    void enable(GPUContext* gpuContext);
    void disable();
    void terminate();
    void addVertexBuffer(GPUContext* gpuContext, const GPUVertexBufferData& data, GPU::u32 size, bool isStatic);
    GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data);
    const GPUVertexBuffer& getVertexBuffer(const GPUVertexBufferData& data) const;
    bool containsVertexBuffer(const GPUVertexBufferData& data) const;
    void setIndicesBuffer(GPUContext* gpuContext, const GPUDataType& gpuDataType, GPU::u32 size, bool isStatic);

private:
    static GPU::u32 findIndex(const std::unordered_map<Core::HashedString, GPU::u32>& indexMap, const Core::HashedString& name);

private:
    GPU::u32 mVertexBufferLayoutId = 0;
    GPUIndicesBuffer mIndicesBuffer;
    GPU::u32 mAttributeIndex = 0;
    std::vector<GPUVertexBuffer> mVertexBuffers;
    std::unordered_map<Core::HashedString, GPU::u32> mVertexBuffersMap;

public:
    CRGET(VertexBuffers)
    RGET(IndicesBuffer)
};

class GPUUniformBuffersContainer
{
public:
	GPUUniformBuffersContainer() = default;

    void terminate();
    void addUniformBuffer(GPUContext* gpuContext, const GPUUniformBufferData& data, GPU::u32 size, bool isStatic);
    GPUUniformBuffer& getUniformBuffer(const GPUUniformBufferData& data);
    const GPUUniformBuffer& getUniformBuffer(const GPUUniformBufferData& data) const;
    GPUUniformBuffer& getUniformBuffer(Core::HashedString bufferName);
    const GPUUniformBuffer& getUniformBuffer(Core::HashedString bufferName) const;
    bool containsUniformBuffer(const GPUUniformBufferData& data) const;

private:
    static GPU::u32 findIndex(const std::unordered_map<Core::HashedString, GPU::u32>& indexMap, const Core::HashedString& name);

private:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::unordered_map<Core::HashedString, GPU::u32> mUniformBuffersMap;
public:
    CRGET(UniformBuffers)
};
