#ifndef MESH_HPP
#define MESH_HPP

#include "Core/Module.hpp"

class BoneVertexData
{
public:
	inline static const u32 smMaxBonesPerVertex = 4;

    i32 mBoneIDs[smMaxBonesPerVertex] = {-1};
    f32 mBoneWeights[smMaxBonesPerVertex] = {0.0f};

    void setBoneWeight(i32 id, f32 weight);
};

class Model;

class Mesh: public ObjectBase
{
    GENERATE_METADATA(Mesh)

public:
    void init(u32 vertexCount, u32 facesCount);

    void addBoneWeight(u32 vertexId, i32 id, f32 weight);

    void clear();

public:
	std::vector<Vector3> mPositions;
	std::vector<Vector3> mNormals;
	std::vector<Vector2> mTextureCoordinates;
	std::vector<Vector4> mColors;
    std::vector<Face> mFaces;
	std::vector<BoneVertexData> mBonesVertexData;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    std::string mMaterialPath;

    Ptr<const Model> mModel;

	ADD_TO_VECTOR(Positions)
	ADD_TO_VECTOR(Normals)
	ADD_TO_VECTOR(TextureCoordinates)
	ADD_TO_VECTOR(Colors)
	ADD_TO_VECTOR(Faces)
	ADD_TO_VECTOR(BonesVertexData)
};

#endif