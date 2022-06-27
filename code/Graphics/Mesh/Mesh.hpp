#ifndef MESH_HPP
#define MESH_HPP

#include "Core/Module.hpp"

class VertexBoneData
{
public:
	inline static const u32 smMaxBonesPerVertex = 4;

    u32 mBoneID[smMaxBonesPerVertex] = {0};
    f32 mBoneWeight[smMaxBonesPerVertex] = {0.0f};
};

class Mesh: public ObjectBase
{
    GENERATE_METADATA(Mesh)

public:
    ~Mesh() override;
    void init(u32 vertexCount, u32 facesCount);
    void addVertex(const Vector3& vector);
    void addNormal(const Vector3& vector);
    void addTexCoord(f32 u, f32 v);
    void addColor(f32 r, f32 g, f32 b, f32 a);
    void addFace(u32 v1, u32 v2, u32 v3);
    void addBone(const VertexBoneData& bone);
    void addFaceIndex(u32 i);
    void addVertices(const std::vector<f32> vec);
    void addNormals(const std::vector<f32> vec);
    void addTextureCoordinates(const std::vector<f32> vec);
    void addColors(const std::vector<f32> vec);
    void addFaces(const std::vector<u32> vec);
    void addBones(const std::vector<VertexBoneData> vec);
    void copyVertices(Ptr<const Mesh> other);
    void copyNormals(Ptr<const Mesh> other);
    void copyTextureCoordinates(Ptr<const Mesh> other);
    void copyColors(Ptr<const Mesh> other);
    void copyFaces(Ptr<const Mesh> other);
    void copyBones(Ptr<const Mesh> other);
    void clear();

private:
	std::vector<f32> mVertices;
	std::vector<f32> mNormals;
	std::vector<f32> mTextureCoordinates;
	std::vector<f32> mColors;

    std::vector<u32> mFaces;

	std::vector<VertexBoneData> mBones;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    std::string mMaterialPath;

public:
	inline static const u32 smVertexPositionSize = 3;
	inline static const u32 smVertexNormalSize = 3;
	inline static const u32 smVertexTexCoordSize = 2;
	inline static const u32 smVertexColorSize = 4;
	inline static const u32 smFaceSize = 3;

	inline static const u32 smBoneIDSize = 1;
    inline static const u32 smBoneWeightSize = 1;

	CRGET(Vertices)
	CRGET(Normals)
	CRGET(TextureCoordinates)
	CRGET(Colors)
	CRGET(Faces)
    CRGET(Bones)
	GET(VertexCount)
	GET(FacesCount)

    CRGET_SET(MaterialPath)
};

#endif