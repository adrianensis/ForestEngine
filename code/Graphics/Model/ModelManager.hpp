#pragma once

#include "Core/Minimal.hpp"
#include "Core/System/System.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "GPU/Mesh/GPUMesh.hpp"

class Model;

class ModelManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    WeakPtr<const Model> loadModel(const std::string& path);
    void setMeshToModel(WeakPtr<const GPUMesh> mesh, WeakPtr<Model> model);
    WeakPtr<Model> getModelFromMesh(WeakPtr<const GPUMesh> mesh) const;

private:
    std::unordered_map<std::string, OwnerPtr<Model>> mModels;
    std::unordered_map<WeakPtr<const GPUMesh>, WeakPtr<Model>> mMeshToModels;
    PoolHandler<Material> mDefaultModelMaterial;
public:
    GET(DefaultModelMaterial)
    CRGET(Models)
    CRGET(MeshToModels)
};
REGISTER_CLASS(ModelManager);
