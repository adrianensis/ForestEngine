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
    static GPU::u32 findIndex(const std::unordered_map<std::string, GPU::u32>& indexMap, const std::string& name);

private:
    GPU::u32 mVertexBufferLayoutId = 0;
    GPUIndicesBuffer mIndicesBuffer;
    GPU::u32 mAttributeIndex = 0;
    std::vector<GPUVertexBuffer> mVertexBuffers;
    std::unordered_map<std::string, GPU::u32> mVertexBuffersMap;

public:
    const auto& getVertexBuffers() const { return mVertexBuffers; }
    auto& getIndicesBuffer() { return mIndicesBuffer; }
};

class GPUUniformBuffersContainer
{
public:
	GPUUniformBuffersContainer() = default;

    void terminate();
    void addUniformBuffer(GPUContext* gpuContext, const GPUUniformBufferData& data, GPU::u32 size, bool isStatic);
    GPUUniformBuffer& getUniformBuffer(const GPUUniformBufferData& data);
    const GPUUniformBuffer& getUniformBuffer(const GPUUniformBufferData& data) const;
    GPUUniformBuffer& getUniformBuffer(std::string bufferName);
    const GPUUniformBuffer& getUniformBuffer(std::string bufferName) const;
    bool containsUniformBuffer(const GPUUniformBufferData& data) const;

private:
    static GPU::u32 findIndex(const std::unordered_map<std::string, GPU::u32>& indexMap, const std::string& name);

private:
    std::vector<GPUUniformBuffer> mUniformBuffers;
    std::unordered_map<std::string, GPU::u32> mUniformBuffersMap;
public:
    const auto& getUniformBuffers() const { return mUniformBuffers; }
};
