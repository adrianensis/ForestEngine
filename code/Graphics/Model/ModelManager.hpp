#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Material/MaterialManager.hpp"

class Model;

class ModelManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    Ptr<const Model> loadModel(const std::string& path);
    void setMeshToModel(Ptr<const Mesh> mesh, Ptr<Model> model);
    Ptr<Model> getModelFromMesh(Ptr<const Mesh> mesh) const;

private:
    std::unordered_map<std::string, OwnerPtr<Model>> mModels;
    std::unordered_map<Ptr<const Mesh>, Ptr<Model>> mMeshToModels;
    PoolHandler<Material> mDefaultModelMaterial;
public:
    GET(DefaultModelMaterial)
    CRGET(Models)
    CRGET(MeshToModels)
};
REGISTER_CLASS(ModelManager);
