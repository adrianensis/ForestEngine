#pragma once

#include "Engine/Minimal.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

class GPUMesh: public ObjectBase
{
public:
    void init(u32 vertexCount, u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers);
    void setColor(const Vector4& color);
    void clear();

public:

    std::vector<GPUVariableData> mGPUVertexInputBuffers;
    std::unordered_map<std::string, TypedByteBuffer> mBuffers;
    TypedByteBuffer mIndices = TypedByteBuffer(sizeof(Face));

	u32 mVertexCount = 0;
	u32 mIndicesCount = 0;

    Vector3 mMin;
    Vector3 mMax;
};
REGISTER_CLASS(GPUMesh);
