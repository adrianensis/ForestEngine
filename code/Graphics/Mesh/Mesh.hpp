#ifndef MESH_HPP
#define MESH_HPP

#include "Core/Module.hpp"

class BoneVertexData
{
public:
	inline static const u32 smMaxBonesPerVertex = 4;

    i32 mBoneIDs[smMaxBonesPerVertex] = {-1};
    f32 mBoneWeights[smMaxBonesPerVertex] = {0.0f};

    void addBoneData(i32 id, f32 weight);
};

class Model;

class Mesh: public ObjectBase
{
    GENERATE_METADATA(Mesh)

public:
    ~Mesh() override;
    void init(u32 vertexCount, u32 facesCount);
    void addPosition(const Vector3& pos);
    void addNormal(const Vector3& vector);
    void addTexCoord(const Vector2& texCoord);
    void addColor(const Vector4& color);
    void addFace(const Face& face);
    void addBoneVertexData(const BoneVertexData& bone);

    void addPositions(const std::vector<Vector3>& vec);
    void addNormals(const std::vector<f32>& vec);
    void addTextureCoordinates(const std::vector<Vector2>& vec);
    void addColors(const std::vector<Vector4>& vec);
    void addFaces(const std::vector<Face>& vec);
    void addBonesVertexData(const std::vector<BoneVertexData>& vec);

    void addBoneWeight(u32 vertexId, i32 id, f32 weight);

    void copyPositions(Ptr<const Mesh> other);
    void copyNormals(Ptr<const Mesh> other);
    void copyTextureCoordinates(Ptr<const Mesh> other);
    void copyColors(Ptr<const Mesh> other);
    void copyFaces(Ptr<const Mesh> other);
    void copyBones(Ptr<const Mesh> other);
    void clear();

private:
	std::vector<Vector3> mPositions;
	std::vector<f32> mNormals;
	std::vector<Vector2> mTextureCoordinates;
	std::vector<Vector4> mColors;

    std::vector<Face> mFaces;

	std::vector<BoneVertexData> mBonesVertexData;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    std::string mMaterialPath;

    Ptr<const Model> mModel;

public:
	inline static const u32 smVertexNormalSize = 3;

	inline static const u32 smBoneIDSize = 1;
    inline static const u32 smBoneWeightSize = 1;

	CRGET(Positions)
	CRGET(Normals)
	CRGET(TextureCoordinates)
	CRGET(Colors)
	CRGET(Faces)
    CRGET(BonesVertexData)
	GET(VertexCount)
	GET(FacesCount)
    CGET_SET(Model)

    CRGET_SET(MaterialPath)
};

#endif