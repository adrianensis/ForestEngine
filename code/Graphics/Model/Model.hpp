#ifndef MODEL_HPP
#define MODEL_HPP

#include "Core/Module.hpp"
#include "assimp/Importer.hpp"

class Animation;

class BoneData
{
public:
    u32 mId = 0;
    // offset matrix transforms vertex from model space to bone space
	Matrix4 mOffsetMatrix;
};

class Mesh;

class Model: public ObjectBase
{
    GENERATE_METADATA(Model)

public:
    ~Model() override;

    void init(const std::string& path);
    bool isAnimated() const { return mBonesIndexCount > 0 && !mAnimations.empty(); }

private:
    Assimp::Importer mImporter;
    std::string mPath;
    std::vector<OwnerPtr<Mesh>> mMeshes;
    Matrix4 mGlobalInverseTransform;

    std::map<std::string, BoneData> mBonesMapping;
    u32 mBonesIndexCount = 0;

    std::vector<OwnerPtr<Animation>> mAnimations;

public:
    CRGET(Meshes)
    CRGET(Path)
    CRGET(Importer)
    CRGET(GlobalInverseTransform)
    CRGET(BonesMapping)
    GET(BonesIndexCount)
};

#endif