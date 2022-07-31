#ifndef MODEL_HPP
#define MODEL_HPP

#include "Core/Module.hpp"
#include "assimp/Importer.hpp"

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

public:
    CRGET(Meshes)
    CRGET(Importer)
    CRGET(GlobalInverseTransform)
};

#endif