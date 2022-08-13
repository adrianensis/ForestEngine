#ifndef MESH_HPP
#define MESH_HPP

#include "Core/Module.hpp"
#include "assimp/scene.h" // Output data structure

class BoneVertexData
{
public:
	inline static const u32 smMaxBonesPerVertex = 4;

    i32 mBoneIDs[smMaxBonesPerVertex] = {-1};
    f32 mBoneWeights[smMaxBonesPerVertex] = {0.0f};

    void addBoneData(i32 id, f32 weight);
};

class Model;
struct aiNodeAnim;
struct aiNode;
struct aiAnimation;

class Mesh: public ObjectBase
{
    GENERATE_METADATA(Mesh)

public:
    ~Mesh() override;
    void init(u32 vertexCount, u32 facesCount);
    void addVertex(CR(Vector3) vector);
    void addNormal(CR(Vector3) vector);
    void addTexCoord(f32 u, f32 v);
    void addColor(f32 r, f32 g, f32 b, f32 a);
    void addFace(u32 v1, u32 v2, u32 v3);
    void addBoneVertexData(CR(BoneVertexData) bone);
    void addFaceIndex(u32 i);

    void addVertices(const std::vector<f32>& vec);
    void addNormals(const std::vector<f32>& vec);
    void addTextureCoordinates(const std::vector<f32>& vec);
    void addColors(const std::vector<f32>& vec);
    void addFaces(const std::vector<u32>& vec);
    void addBonesVertexData(const std::vector<BoneVertexData>& vec);

    void addBoneWeight(u32 vertexId, i32 id, f32 weight);

    void copyVertices(Ptr<const Mesh> other);
    void copyNormals(Ptr<const Mesh> other);
    void copyTextureCoordinates(Ptr<const Mesh> other);
    void copyColors(Ptr<const Mesh> other);
    void copyFaces(Ptr<const Mesh> other);
    void copyBones(Ptr<const Mesh> other);
    void clear();

    void getBoneTransforms(float TimeInSeconds, std::vector<Matrix4>& Transforms) const;

private:
    const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string& nodeName) const;
    void readNodeHierarchy(float animationTimeTicks, const aiNode* pNode, const Matrix4& parentTransform, std::vector<Matrix4>& currentTransforms) const;
    u32 FindPosition(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    u32 FindRotation(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    u32 FindScaling(float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTimeTicks, const aiNodeAnim* pNodeAnim) const;

private:
	std::vector<f32> mVertices;
	std::vector<f32> mNormals;
	std::vector<f32> mTextureCoordinates;
	std::vector<f32> mColors;

    std::vector<u32> mFaces;

	std::vector<BoneVertexData> mBonesVertexData;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;

    std::string mMaterialPath;

    Ptr<const Model> mModel;

    mutable u32 test = 0;

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
    CRGET(BonesVertexData)
	GET(VertexCount)
	GET(FacesCount)
    SET(Model)

    CRGET_SET(MaterialPath)
};

#endif