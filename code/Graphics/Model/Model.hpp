#ifndef MODEL_HPP
#define MODEL_HPP

#include "Core/Module.hpp"

class Mesh;

class Model: public ObjectBase
{
    GENERATE_METADATA(Model)

public:
    ~Model() override;

    void init(CR(std::string) path);

private:
    std::string mPath;
    std::vector<OwnerPtr<Mesh>> mMeshes;

public:
    CRGET(Meshes)
};

#endif