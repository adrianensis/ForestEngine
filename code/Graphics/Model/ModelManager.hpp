#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"

class Model;

class ModelManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    Ptr<const Model> loadModel(const std::string& path);
    void setMeshToModel(Ptr<const GPUMesh> mesh, Ptr<Model> model);
    Ptr<Model> getModelFromMesh(Ptr<const GPUMesh> mesh) const;

private:
    std::unordered_map<std::string, OwnerPtr<Model>> mModels;
    std::unordered_map<Ptr<const GPUMesh>, Ptr<Model>> mMeshToModels;
    PoolHandler<Material> mDefaultModelMaterial;
public:
    GET(DefaultModelMaterial)
    CRGET(Models)
    CRGET(MeshToModels)
};
REGISTER_CLASS(ModelManager);
