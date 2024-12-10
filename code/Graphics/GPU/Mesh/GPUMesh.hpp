#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/GPU/Buffer/GPUVertexBuffer.hpp"
#include "Graphics/GPU/Buffer/GPUBuffersContainer.hpp"
#include "Graphics/GPU/Core/GPUBuiltIn.hpp"

class GPUMesh
{
public:
    void init(u32 vertexCount, u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers);
    void setColor(const Vector4& color);
    void clear();
    bool hasVertexInputBuffer(const GPUVariableData& data) const;
    void populateGPUVertexBuffersContainer(GPUVertexBuffersContainer& gpuVertexBuffersContainer, bool isStatic) const;

public:

    std::vector<GPUVariableData> mGPUVertexInputBuffers;
    std::unordered_map<HashedString, TypedByteBuffer> mBuffers;
    TypedByteBuffer mIndices = TypedByteBuffer(sizeof(Face));

	u32 mVertexCount = 0;
	u32 mIndicesCount = 0;

    Vector3 mMin;
    Vector3 mMax;

	u32 mMeshID = 0;

protected:
    // Important: starts by 1, 0 is reserved for null
	inline static u32 smMeshIdCounter = 1;
};
REGISTER_CLASS(GPUMesh);
