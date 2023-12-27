#pragma once

#include "Core/Module.hpp"

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
    void init(u32 vertexCount, u32 facesCount);
    void setColor(const Vector4& color);
    void clear();

public:
    inline static const u32 MAX_BONE_INFLUENCE = smMaxBonesPerVertex;
    inline static const u32 MAX_BONES = 50;

	ByteBuffer mPositions;
    ByteBuffer mNormals;
	ByteBuffer mTextureCoordinates;
	ByteBuffer mColors;
    ByteBuffer mFaces;
	ByteBuffer mBonesVertexIDsData;
	ByteBuffer mBonesVertexWeightsData;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    Vector3 mMin;
    Vector3 mMax;
};
