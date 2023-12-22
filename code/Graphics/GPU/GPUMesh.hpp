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

    void addBoneWeight(u32 vertexId, i32 id, f32 weight);
    void setColor(const Vector4& color);

    void clear();

public:

    inline static const u32 MAX_BONE_INFLUENCE = smMaxBonesPerVertex;
    inline static const u32 MAX_BONES = 50;

	std::vector<Vector3> mPositions;
	std::vector<Vector3> mNormals;
	std::vector<Vector2> mTextureCoordinates;
	std::vector<Vector4> mColors;
    std::vector<Face> mFaces;
	std::vector<BoneVertexIDsData> mBonesVertexIDsData;
	std::vector<BoneVertexWeightsData> mBonesVertexWeightsData;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    Vector3 mMin;
    Vector3 mMax;

	ADD_TO_VECTOR(Positions)
	ADD_TO_VECTOR(Normals)
	ADD_TO_VECTOR(TextureCoordinates)
	ADD_TO_VECTOR(Colors)
	ADD_TO_VECTOR(Faces)
	ADD_TO_VECTOR(BonesVertexIDsData)
	ADD_TO_VECTOR(BonesVertexWeightsData)
};
