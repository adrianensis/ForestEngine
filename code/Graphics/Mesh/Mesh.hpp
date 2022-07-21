#ifndef MESH_HPP
#define MESH_HPP

#include "Core/Module.hpp"

class BoneVertexData
{
public:
	inline static const u32 smMaxBonesPerVertex = 4;

    u32 mBoneIDs[smMaxBonesPerVertex] = {0};
    f32 mBoneWeights[smMaxBonesPerVertex] = {0.0f};

    void addBoneData(u32 id, f32 weight)
    {
        FOR_RANGE(i, 0, smMaxBonesPerVertex)
        {
            if (mBoneWeights[i] == 0.0)
            {
                mBoneIDs[i] = id;
                mBoneWeights[i] = weight;
                //printf("bone %d weight %f index %i\n", id, weight, i);
                return;
            }
        }

        // should never get here - more bones than we have space for
        //ASSERT_MSG(false, "should never get here - more bones than we have space for");
    }
};

class BoneData
{
public:
	Matrix4 mOffsetMatrix;
};

struct aiNodeAnim;
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

    void addVertices(const std::vector<f32> vec);
    void addNormals(const std::vector<f32> vec);
    void addTextureCoordinates(const std::vector<f32> vec);
    void addColors(const std::vector<f32> vec);
    void addFaces(const std::vector<u32> vec);
    void addBonesVertexData(const std::vector<BoneVertexData> vec);

    u32 registerBone(CR(std::string) name);
    bool isBoneRegistered(CR(std::string) name) const;
    u32 getBoneID(CR(std::string) name) const;
    void setBoneOffsetMatrix(u32 id, CR(Matrix4) offsetMatrix);
    void addBoneWeight(u32 vertexId, u32 id, f32 weight);

    void copyVertices(Ptr<const Mesh> other);
    void copyNormals(Ptr<const Mesh> other);
    void copyTextureCoordinates(Ptr<const Mesh> other);
    void copyColors(Ptr<const Mesh> other);
    void copyFaces(Ptr<const Mesh> other);
    void copyBones(Ptr<const Mesh> other);
    void clear();

// private:
//     const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string& nodeName);
//     void readNodeHierarchy(float animationTimeTicks, const aiNode* pNode, const Matrix4& parentTransform);

private:
	std::vector<f32> mVertices;
	std::vector<f32> mNormals;
	std::vector<f32> mTextureCoordinates;
	std::vector<f32> mColors;

    std::vector<u32> mFaces;

	std::vector<BoneVertexData> mBonesVertexData;
    std::vector<BoneData> mBonesData;

    std::map<std::string, u32> mBonesNameIndexMap;

	u32 mVertexCount = 0;
	u32 mFacesCount = 0;
    u32 mBonesIndexCount = 0;

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
    CRGET(BonesVertexData)
	GET(VertexCount)
	GET(FacesCount)

    CRGET_SET(MaterialPath)
};

#endif