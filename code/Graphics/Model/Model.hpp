#ifndef MODEL_HPP
#define MODEL_HPP

#include "Core/Module.hpp"
#include "assimp/Importer.hpp"

class BoneData
{
public:
    u32 mId = 0;
	Matrix4 mOffsetMatrix;
};

class Mesh;

class Model: public ObjectBase
{
    GENERATE_METADATA(Model)

public:
    ~Model() override;

    void init(CR(std::string) path);

private:
    Assimp::Importer mImporter;
    std::string mPath;
    std::vector<OwnerPtr<Mesh>> mMeshes;
    Matrix4 mGlobalInverseTransform;

    std::map<std::string, BoneData> mBonesMapping;
    u32 mBonesIndexCount = 0;

public:
    CRGET(Meshes)
    CRGET(Importer)
    CRGET(GlobalInverseTransform)
    CRGET(BonesMapping)
    GET(BonesIndexCount)
};

#endif