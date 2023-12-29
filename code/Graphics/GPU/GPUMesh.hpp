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

class GPUVertexInputBuffers
{
public:
    std::vector<GPUVariableData> mBuffers;
};

class GPUMesh: public ObjectBase
{
    GENERATE_METADATA(GPUMesh)

public:
    void init(u32 vertexCount, u32 facesCount);
    void setColor(const Vector4& color);
    void clear();

public:
    inline static const u32 MAX_BONE_INFLUENCE = smMaxBonesPerVertex;
    inline static const u32 MAX_BONES = 50;
    inline static const GPUVertexInputBuffers smDefaultGPUVertexInputBuffers =
    {
        {
            GPUBuiltIn::VertexInput::mPosition,
            GPUBuiltIn::VertexInput::mTextureCoord,
            GPUBuiltIn::VertexInput::mColor,
            GPUBuiltIn::VertexInput::mNormal,
            GPUBuiltIn::VertexInput::mBonesIDs,
            GPUBuiltIn::VertexInput::mBonesWeights,
        }
    };

    GPUVertexInputBuffers mGPUVertexInputBuffers;

	ByteBuffer mPositions = ByteBuffer(sizeof(Vector3));
	ByteBuffer mTextureCoordinates = ByteBuffer(sizeof(Vector2));
	ByteBuffer mColors = ByteBuffer(sizeof(Vector4));
    ByteBuffer mNormals = ByteBuffer(sizeof(Vector3));
    ByteBuffer mFaces = ByteBuffer(sizeof(Face));
	ByteBuffer mBonesVertexIDsData = ByteBuffer(sizeof(BoneVertexIDsData));
	ByteBuffer mBonesVertexWeightsData = ByteBuffer(sizeof(BoneVertexWeightsData));

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    Vector3 mMin;
    Vector3 mMax;
};
