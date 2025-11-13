#pragma once

#include "GPU/Buffer/GPUVertexBuffer.hpp"
#include "GPU/Buffer/GPUBuffersContainer.hpp"

class GPUMesh
{
public:
    void init(Core::u32 vertexCount, Core::u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers);
    void setColor(const Maths::Vector4& color);
    void clear();
    bool hasVertexInputBuffer(const GPUVariableData& data) const;

public:

    std::vector<GPUVariableData> mGPUVertexInputBuffers;
    std::unordered_map<Core::HashedString, Core::TByteBuffer> mBuffers;
    Core::TByteBuffer mIndices = Core::TByteBuffer(sizeof(Maths::Face));

	Core::u32 mVertexCount = 0;
	Core::u32 mIndicesCount = 0;

    Maths::Vector3 mMin;
    Maths::Vector3 mMax;

	Core::u32 mMeshID = 0;

protected:
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u32 smMeshIdCounter = 1;
};

