#pragma once

#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"

class GPUMesh
{
public:
    void init(GPU::u32 vertexCount, GPU::u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers);
    void setColor(const Maths::Vector4& color);
    void clear();
    bool hasVertexInputBuffer(const GPUVariableData& data) const;

public:

    std::vector<GPUVariableData> mGPUVertexInputBuffers;
    std::unordered_map<Core::HashedString, Core::TByteBuffer> mBuffers;
    Core::TByteBuffer mIndices = Core::TByteBuffer(sizeof(Maths::Face));

	GPU::u32 mVertexCount = 0;
	GPU::u32 mIndicesCount = 0;

    Maths::Vector3 mMin;
    Maths::Vector3 mMax;

	GPU::u32 mMeshID = 0;

protected:
    // Important: starts by 1, 0 is reserved for null
	inline static GPU::u32 smMeshIdCounter = 1;
};

