#pragma once

#include "Core/Module.hpp"
#include "Graphics/GPU/GPUVertexBuffer.hpp"
#include "Graphics/GPU/GPUBuiltIn.hpp"

inline static const u32 smMaxBonesPerVertex = 4;

class BoneVertexIDsData
{
public:
    i32 mBonesIDs[smMaxBonesPerVertex] = {-1, -1, -1, -1};
};

class BoneVertexWeightsData
{
public:
    f32 mBonesWeights[smMaxBonesPerVertex] = {0.0f, 0.0f, 0.0f, 0.0f};
};

class GPUMesh: public ObjectBase
{
    GENERATE_METADATA(GPUMesh)

public:
    void init(u32 vertexCount, u32 indicesCount, const std::vector<GPUVariableData>& gpuVertexInputBuffers);
    void setColor(const Vector4& color);
    void clear();

public:
    inline static const u32 MAX_BONE_INFLUENCE = smMaxBonesPerVertex;
    inline static const u32 MAX_BONES = 50;

    std::vector<GPUVariableData> mGPUVertexInputBuffers;
    std::unordered_map<std::string, ByteBuffer> mBuffers;
    ByteBuffer mIndices = ByteBuffer(sizeof(Face));

	u32 mVertexCount = 0;
	u32 mIndicesCount = 0;

    Vector3 mMin;
    Vector3 mMax;
};
